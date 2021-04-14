def hello_from_python ():
  return 'Hello from Python!'


def writeResultsToArray():
    my_list = list()
    rcnt = 0
    for x in range(0, 15):
        my_list.append('AAAAAAAAAAAAAAAAAAAAAAAA')
        rcnt += 1
    print('DONE! WROTE', rcnt, 'RESULTS.')
    return my_list

get_dir_list.get_dir_list(os.listdir('.'))
print(writeResultsToArray())


""" Birdnet file code
def writeResultsToFile(detections, min_conf, path):

    print('WRITING RESULTS TO', path, '...', end=' ')
    rcnt = 0
    with open(path, 'w') as rfile:
        rfile.write('Start (s);End (s);Scientific name;Common name;Confidence\n')
        for d in detections:
            for entry in detections[d]:
                if entry[1] >= min_conf and (entry[0] in WHITE_LIST or len(WHITE_LIST) == 0):
                    rfile.write(d + ';' + entry[0].replace('_', ';') + ';' + str(entry[1]) + '\n')
                    rcnt += 1
    print('DONE! WROTE', rcnt, 'RESULTS.')


      for d in detections: << this array contains 

"""