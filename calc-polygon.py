#!/usr/bin/env python3

import configparser
import math
import csv
import ais.stream

def readconfig():
	cameraconfig = configparser.ConfigParser()
	cameraconfig.read('cameraconfig.ini')
	name = cameraconfig['GEODATA']['name']
	lat = float(cameraconfig['GEODATA']['lat'])
	lon = float(cameraconfig['GEODATA']['lon'])
	height = float(cameraconfig['GEODATA']['height'])
	bearing = float(cameraconfig['GEODATA']['bearing'])
	angle = float(cameraconfig['GEODATA']['angle'])
	return name, lat, lon, height, bearing, angle

def horizondist(height):
	sqrtheight = math.sqrt(height)
	dist = 3.57 * sqrtheight 
	return dist

def calcposition(lat1, lon1, brng, dist):
	r = 6378.1
	brng = math.radians(brng)
	lat1 = math.radians(lat1)
	lon1 = math.radians(lon1)
	lat2 = math.asin( math.sin(lat1)*math.cos(dist/r) + 
	     math.cos(lat1)*math.sin(dist/r)*math.cos(brng))
	lon2 = lon1 + math.atan2(math.sin(brng)*math.sin(dist/r)*math.cos(lat1),
             math.cos(dist/r)-math.sin(lat1)*math.sin(lat2))
	lat2 = math.degrees(lat2)
	lon2 = math.degrees(lon2)
	return lat2, lon2

class Polygon(object):						# from http://www.coderstalk.co/2016/02/04/geofence_and_its_implementation_in_python.html

    def __str__(self):
        return "%s: %s" % (self.__shape_type, self.__shape)

    def __init__(self):
        self.__shape = {}
        self.__shape_type = 'polygon'

    def set_shape(self, poly):
        """Creates a polygon.

        Keyword arguments:
        poly -- should be tuple of tuples contaning vertex in (x, y) form.
            e.g. (  (0,0),
                    (0,1),
                    (1,1),
                    (1,0),
                )
        """
        assert type(()) == type(poly), "argument must be tuple of tuples."
        assert len(poly) > 1, "polygon must have more than one vertex."

        self.__shape['vertex_list'] = poly
        self.__shape['vertex_count'] = len(poly)


    def is_inside(self, point):
        """Returns True if the point lies inside the polygon, False otherwise.
        Works on Ray Casting Method (https://en.wikipedia.org/wiki/Point_in_polygon)

        Keyword arguments:
        point -- a tuple representing the coordinates of point to be tested in (x ,y) form.
        """
        poly = self.__shape['vertex_list']
        n = self.__shape['vertex_count']
        x, y = point
        inside = False

        p1x,p1y = poly[0]
        for i in range(n+1):
            p2x,p2y = poly[i % n]
            if y > min(p1y,p2y):
                if y <= max(p1y,p2y):
                    if x <= max(p1x,p2x):
                        if p1y != p2y:
                            xints = (y-p1y)*(p2x-p1x)/float(p2y-p1y)+p1x
                        if p1x == p2x or x <= xints:
                            inside = not inside
            p1x,p1y = p2x,p2y

        return inside

name, lat1, lon1, height, bearing, angle = readconfig() 	# einlesen der Kameradaten
disthorizon = horizondist(height)				# berechnen der entfernung des horizonts

bearing3 = bearing - angle / 2					# berechnen des kameraöffnungswinkels
bearing4 = bearing + angle / 2

lat3, lon3 = calcposition(lat1, lon1, bearing3, disthorizon)	# berechnen des li eckpunkt des polygons
lat4, lon4 = calcposition(lat1, lon1, bearing4, disthorizon)	# berechnen des re eckpunkt des polygons

with open('camerapolygon.csv', 'w', newline='') as csvfile:		# schreiben der camerapolygon.csv --> nur für testzwecke notwendig
	outputwriter = csv.writer(csvfile, delimiter=',')
	data = [['lat','lon','description'],
		[lat1,lon1,name],
		[lat3,lon3,'left-corner'],
		[lat4,lon4,'right-corner']]
	outputwriter.writerows(data)

origin = (lat1, lon1);
cornerleft = (lat3, lon3);
cornerright = (lat4, lon4);
visiblepoly = (origin, cornerleft, cornerright);
print('theoretisch sichtbarer bereich, ausgedrückt als tupel',visiblepoly)

foo = Polygon()
foo.set_shape(visiblepoly)

linenumber = 0
insidecount = 0
outsidecount = 0

with open("nmea-sample", 'r') as inf:
	for msg in ais.stream.decode(inf):
		linenumber = linenumber +1
		xcoord = float(msg.get('x',0.00))
		ycoord = float(msg.get('y',0.00))
		pos = (xcoord,ycoord);
		innendrin = foo.is_inside(pos)
		if innendrin == True:
			print('Line:', linenumber, ' Ship is inside Poly?',innendrin)
			insidecount = insidecount + 1
		elif innendrin == False:
			outsidecount = outsidecount + 1
			
		# print(' ################# NEW PACKET ################## ')
		# print(msg)
		# print('Line:', linenumber, ' Shipposition', msg['mmsi'], xcoord, ycoord)
		# print('Line:', linenumber, ' Ship is inside poly?',foo.is_inside(pos))

print('Ships outside of Polygon: ', outsidecount)
print('Ships inside of Polygon: ', insidecount)
print('Last linenumber', linenumber)

