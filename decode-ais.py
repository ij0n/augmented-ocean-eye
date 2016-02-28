#!/usr/bin/env python3

import ais.stream
import csv

with open('nmea-sample.csv', 'a') as myfile:
	myfile.write("lat,lon,description\n")

with open("nmea-sample", 'r') as inf:
	for msg in ais.stream.decode(inf):
		xcoord = msg.get('x',181.00)
		ycoord = msg.get('y',91.00)		
		print(xcoord,',',ycoord,',',msg['mmsi'])
		newline = str(xcoord) + ',' + str(ycoord)
		newline += ',' + str(msg['mmsi']) + "\n"
		# print('Ship', msg['mmsi'], msg['x'], msg['y'])
		with open('nmea-sample.csv', 'a') as myfile:
			myfile.write(newline)
		
# next step: repair this so default values are used like in the other file if no value is present.
# then check on umap




# with open('aisoutput.csv', 'w') as outf:
#	w = csv.DictWriter(outf, my_dict.keys())
#	w.writeheader()
#	w.writerow(my_dict)
