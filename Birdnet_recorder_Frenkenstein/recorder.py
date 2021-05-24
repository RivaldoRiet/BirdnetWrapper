import os
import time
import json
import operator
from threading import Thread

import numpy as np
import pyaudio
import datetime
import math

from tensorflow import lite as tflite

from config import config as cfg
from utils import audio
from utils import image
from utils import log

DET = {}
FRAMES = np.array([], dtype='float32')
INTERPRETER = None
INPUT_LAYER_INDEX = -1
OUTPUT_LAYER_INDEX = -1

def openStream():       

    try:

        # Setup pyaudio
        paudio = pyaudio.PyAudio()
        info = paudio.get_host_api_info_by_index(0)
        numdevices = info.get('deviceCount')
        for i in range(0, numdevices):
            if (paudio.get_device_info_by_host_api_device_index(0, i).get('maxInputChannels')) > 0:
                print("Input Device id ", i, " - ", paudio.get_device_info_by_host_api_device_index(0, i).get('name'))
        # Stream Settings
        stream = paudio.open(format=pyaudio.paFloat32,
                            input_device_index=0,
                            channels=1,
                            rate=cfg['SAMPLE_RATE'],
                            input=True,
                            frames_per_buffer=cfg['SAMPLE_RATE'] // 2)

        return stream

    except:
        return None

def record():

    global FRAMES

    # Open stream
    stream = openStream()
    
    while not cfg['KILL_ALL']:

        try:

            # Read from stream
            data = stream.read(cfg['SAMPLE_RATE'] // 2)
            data = np.fromstring(data, 'float32');
            FRAMES = np.concatenate((FRAMES, data))

            # Truncate frame count
            FRAMES = FRAMES[-int(cfg['SAMPLE_RATE'] * cfg['SPEC_LENGTH']):]

        except KeyboardInterrupt:
            cfg['KILL_ALL'] = True
            break
        except:
            FRAMES = np.array([], dtype='float32')
            stream = openStream()
            continue

def loadModel(model_file, config_file):

    global INPUT_LAYER_INDEX
    global OUTPUT_LAYER_INDEX
    global MDATA_INPUT_INDEX
    global CLASSES

    # Load TFLite model and allocate tensors.
    #interpreter = tflite.Interpreter(model_path='model.tflite')
    interpreter = tflite.Interpreter(model_path='model.tflite')
    interpreter.allocate_tensors()

    # Get input and output tensors.
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    # Get input tensor index
    INPUT_LAYER_INDEX = input_details[0]['index']
    MDATA_INPUT_INDEX = input_details[1]['index']
    OUTPUT_LAYER_INDEX = output_details[0]['index']

    # Load labels
    CLASSES = []
    with open('labels.txt', 'r') as lfile:
        for line in lfile.readlines():
            CLASSES.append(line.replace('\n', ''))

    return interpreter    

def getSpeciesList():

    # Add selected species to white list
    cfg['WHITE_LIST'] = [# Species that have a sound file
                         'Sturnus vulgaris_European Starling',
                         'Delichon urbicum_Common House-Martin',
                         'Linaria cannabina_Eurasian Linnet',
                         'Ficedula hypoleuca_European Pied Flycatcher',
                         'Regulus regulus_Goldcrest',
                         'Emberiza citrinella_Yellowhammer',
                         'Cyanistes caeruleus_Eurasian Blue Tit',
                         'Phylloscopus collybita_Common Chiffchaff',
                         'Carduelis carduelis_European Goldfinch',
                         # Additional species
                         'Parus major_Great Tit',
                         'Passer domesticus_House Sparrow',
                         'Erithacus rubecula_European Robin',
                         'Phoenicurus ochruros_Black Redstart',
                         'Fringilla coelebs_Common Chaffinch',
                         'Turdus merula_Eurasian Blackbird'
                        ]

def convertMetadata(m):

    # Convert week to cosine
    if m[2] >= 1 and m[2] <= 48:
        m[2] = math.cos(math.radians(m[2] * 7.5)) + 1 
    else:
        m[2] = -1

    # Add binary mask
    mask = np.ones((3,))
    if m[0] == -1 or m[1] == -1:
        mask = np.zeros((3,))
    if m[2] == -1:
        mask[2] = 0.0

    return np.concatenate([m, mask])

def getInput(sig):

    if cfg['INPUT_TYPE'] == 'raw':        

        # Prepare as input
        sample = audio.prepare(sig)

    else:        
        spec = audio.getSpec(sig,
                            rate=cfg['SAMPLE_RATE'],
                            fmin=cfg['SPEC_FMIN'],
                            fmax=cfg['SPEC_FMAX'],
                            win_len=cfg['WIN_LEN'],
                            spec_type=cfg['SPEC_TYPE'],
                            magnitude_scale=cfg['MAGNITUDE_SCALE'],
                            bandpass=True,
                            shape=(64, 384))

        # DEBUG: Save spec?
        if cfg['DEBUG_MODE']:
            image.saveSpec(spec, os.path.join(cfg['LOG_DIR'], 'spec.jpg'))

        # Prepare as input
        sample = image.prepare(spec)

    return sample

def splitSignal(sig, rate, overlap, seconds=3.0, minlen=1.5):

    # Split signal with overlap
    sig_splits = []
    for i in range(0, len(sig), int((seconds - overlap) * rate)):
        split = sig[i:i + int(seconds * rate)]

        # End of signal?
        if len(split) < int(minlen * rate):
            break
        
        # Signal chunk too short? Fill with zeros.
        if len(split) < int(rate * seconds):
            temp = np.zeros((int(rate * seconds)))
            temp[:len(split)] = split
            split = temp
        
        sig_splits.append(split)

    return sig_splits


def flat_sigmoid(x, sensitivity=-1):
    return 1 / (1.0 + np.exp(sensitivity * x))

def predictionPooling(p, sensitivity=-1, mode='avg'):

    # Apply sigmoid function
    p = flat_sigmoid(p, sensitivity)

    # Mean exponential pooling for monophonic recordings
    if mode == 'mexp':
        p_pool = np.mean((p * 2.0) ** 2, axis=0)

    # Simple average pooling
    else:        
        p_pool = np.mean(p, axis=0)
    
    p_pool[p_pool > 1.0] = 1.0

    return p_pool
    
    
def custom_sigmoid(x, sensitivity=1.0):
    return 1 / (1.0 + np.exp(-sensitivity * x))

def predict(sample, interpreter, sensitivity):

    # Make a prediction
    interpreter.set_tensor(INPUT_LAYER_INDEX, np.array(sample[0], dtype='float32'))
    interpreter.set_tensor(MDATA_INPUT_INDEX, np.array(sample[1], dtype='float32'))
    interpreter.invoke()
    prediction = interpreter.get_tensor(OUTPUT_LAYER_INDEX)[0]

    # Apply custom sigmoid
    p_sigmoid = custom_sigmoid(prediction, sensitivity)

    # Get label and scores for pooled predictions
    p_labels = dict(zip(CLASSES, p_sigmoid))

    # Sort by score
    p_sorted = sorted(p_labels.items(), key=operator.itemgetter(1), reverse=True)

    # Remove species that are on blacklist
    for i in range(min(10, len(p_sorted))):
        if p_sorted[i][0] in ['Human_Human', 'Non-bird_Non-bird', 'Noise_Noise']:
            p_sorted[i] = (p_sorted[i][0], 0.0)

    # Only return first the top ten results
    return p_sorted[:10]
    
    
def analyzeAudioData(chunks,interpreter):

    my_date = datetime.date.today() # current date
    year, week_num, day_of_week = my_date.isocalendar()
    detections = {}
    start = time.time()
    print('ANALYZING AUDIO...', end=' ', flush=True)

    # Convert and prepare metadata
    mdata = convertMetadata(np.array([52.379189, -4.899431, week_num]))
    mdata = np.expand_dims(mdata, 0)

    # Parse every chunk
    pred_start = 0.0
    for c in chunks:

        # Prepare as input signal
        sig = np.expand_dims(c, 0)

        # Make prediction
        p = predict([sig, mdata], interpreter, 1)

        # Save result and timestamp
        pred_end = pred_start + 3.0
        detections[str(pred_start) + ';' + str(pred_end)] = p
        pred_start = pred_end - 0.0

    print('DONE! Time', int((time.time() - start) * 10) / 10.0, 'SECONDS')

    return detections

def analyzeStream(interpreter):

    # Time
    start = time.time()

    # Get signal from FRAMES
    sig = FRAMES.copy()

    # Do we have enough frames?
    if len(sig) < cfg['SAMPLE_RATE'] * cfg['SPEC_LENGTH']:
        return None
    
    print('sig length: ' + str(len(sig)))
    
    # Split audio into 3-second chunks
    chunks = splitSignal(sig, cfg['SAMPLE_RATE'], 0.0)
    print('DONE! READ', str(len(chunks)), 'CHUNKS.')

    my_date = datetime.date.today()  # current date
    year, week_num, day_of_week = my_date.isocalendar()
    detections = {}
    start = time.time()
    print('ANALYZING AUDIO...', end=' ', flush=True)

    # Convert and prepare metadata
    mdata = convertMetadata(np.array([52.379189, -4.899431, week_num]))
    mdata = np.expand_dims(mdata, 0)

    # Parse every chunk
    pred_start = 0.0
    for c in chunks:
        # Prepare as input signal
        sig = np.expand_dims(c, 0)

        # Make prediction
        p = predict([sig, mdata], interpreter, 1)

        # Save result and timestamp
        pred_end = pred_start + 3.0
        detections[str(pred_start) + ';' + str(pred_end)] = p
        pred_start = pred_end - 0.0

    print('DONE! Time', int((time.time() - start) * 10) / 10.0, 'SECONDS')
    
    my_list = list()
    rcnt = 0
    for d in detections:
            for entry in detections[d]:
                if entry[1] >= 0.1:
                    print(d + ';' + entry[0].replace('_', ';') + ';' + str(entry[1]))
                    my_list.append(d + ';' + entry[0].replace('_', ';') + ';' + str(entry[1]))
                    rcnt += 1
    #time.sleep(3)
    return my_list

def run():

    # Load model
    interpreter = loadModel(cfg['MODEL_PATH'], cfg['CONFIG_PATH'])

    # Load species list
    getSpeciesList()

    # Start recording
    log.p(('STARTING RECORDING WORKER'))
    recordWorker = Thread(target=record, args=())
    recordWorker.start()

    # Keep running...
    log.p(('STARTING ANALYSIS'))
    while not cfg['KILL_ALL']:

        try:
            time.sleep(1)
            # Make prediction
            p = analyzeStream(interpreter)

            # Save results
            if not p == None:

                # Sleep if we are too fast
                if 'time_for_prediction' in p:
                    if p['time_for_prediction'] < cfg['SPEC_LENGTH'] - cfg['SPEC_OVERLAP']:
                        time.sleep((cfg['SPEC_LENGTH'] - cfg['SPEC_OVERLAP']) - (p['time_for_prediction']))

            else:
                time.sleep(1.0)

        except KeyboardInterrupt:
            cfg['KILL_ALL'] = True
            break
        #except:
            #cfg.KILL_ALL = True

    # Done
    log.p(('TERMINATED'))


if __name__ == '__main__':    
    
    run()
    