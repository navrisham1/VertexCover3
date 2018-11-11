import sys
import re
import math

# my modules
import ece650_a1_vertex_cover as vertex_cover

################################
# UI - User Interface class
################################
class UI(object):

    #==================================
    def __init__(self):
        self.streets = {}
        self.intersects = {}
        self.debug = False

        self.vc = vertex_cover.VertexCover()    # VertexCover object
        self.vc.debug = self.debug

    #==================================
    def debugOn(self):
        self.debug = True
        self.vc.debug = True

    #==================================
    def debugOff(self):
        self.debug = False
        self.vc.debug = False

    #==================================
    def usage(self):
        print "Enter a commnd ...."
        print 'Examples: '
        print '  To add street:   \ta "Street Name" (2, 3) (2,5) (3,8)'
        print '  To change street:\tc "Street Name" (2, 3) (2,5) (3,8)'
        print '  To remove street:\tr "Street Name"'
        print '  To plot graph:   \tg'
        print ''

    #==================================
    def printError(self, message):
        sys.stderr.write( "Error: (ece650-a1)" + message + "\n")

    #==================================
    def parseCommand(self, line):
        command = None; param1 = None;
        parts = re.split(' ', line, 1)
        if len(parts) == 2:
            command, param1 = parts
        elif len(parts) == 1:
            command = parts[0]

        return command, param1

    #==================================
    def parseStreet(self, param1):
        street = None
        param2 = None

        # only alphabets and spaces can be included
        results = re.findall('^"[ ]*[a-zA-Z.][ a-zA-Z.]*"', param1)
        if len(results) == 1:
            street = results[0]
            param2 = param1.replace(street, '')
        else:
            raise vertex_cover.VC_Exception("Invalid street name in '" + param1 + "'.")

        return street, param2

    #==================================
    def parseCoordinates(self, param2):
        if len(param2)==0:
            raise vertex_cover.VC_Exception("Coordinates missing.")
        elif not re.search('^\s',param2):
            raise vertex_cover.VC_Exception("Coordinates need to be separated from street name by a space.")

        param2 = param2.strip()

        if not re.search('^(\(\s*-?[0-9]+\s*,\s*-?[0-9]+\s*\)\s*)+$',param2):
            raise vertex_cover.VC_Exception("'" + param2 + "'*" + " not valid coordinates.")
        elif re.findall('[^0-9\s\(\),-]+',param2):
            raise vertex_cover.VC_Exception("'" + param2 + "'" + " not valid coordinates.")

        cl = re.findall('^\(.*\)$',param2)
        if len(cl) != 1:
            raise vertex_cover.VC_Exception("'" + param2 + "'" + " not valid coordinates.")

        cord_list = re.findall('\(\s*-?[0-9]+\s*,\s*-?[0-9]+\s*\)', cl[0])
        if len(cord_list) < 2:
            raise vertex_cover.VC_Exception("Too few coordinates in '" + param2 + "'.")

        coordinates=[]
        for cord in cord_list:
            cord = cord.replace('(','')
            cord = cord.replace(')','')
            x, y = re.split(',',cord)
            coordinates.append( [int(x), int(y)] )

        return coordinates

    #==================================
    def parseInput(self, line):
        command = None; street = None; coordinates = [];

        command, param1 = self.parseCommand( line.strip() )
        if command == 'a' or  command == 'c':
            if not param1:
                 raise vertex_cover.VC_Exception("Missing street name for command - '" + command + "'.")
            street, param2 = self.parseStreet( param1.strip() )
            if street:
                coordinates = self.parseCoordinates( param2 )

        elif command == 'r':
            if not param1:
                 raise vertex_cover.VC_Exception("Missing street name for command - '" + command + "'.")
            street, param2 = self.parseStreet( param1.strip() )

        elif command != 'g':
            raise vertex_cover.VC_Exception("Invalid command - '" + command + "'. Valid commands are 'a', 'c', 'r' or 'g'.")

        if self.debug:
            print '========================'
            print "Input: ", line.strip()
            print "Command: ", command
            print "Street: ", street
            print "Coordinates: ", coordinates
            print

        return command, street, coordinates

    #==================================
    def run(self):
        if self.debug:
            self.usage()

        while True:
            line = sys.stdin.readline()
            if line == '': # EOF detection
                return

            try:
                command, street_name, coordinates = self.parseInput(line)

                if command == 'a' and street_name and coordinates:
                    self.vc.addStreet( street_name, coordinates )

                elif command == 'c' and street_name and coordinates:
                    self.vc.changeStreet( street_name, coordinates )

                elif command == 'r' and street_name:
                    self.vc.removeStreet( street_name )

                elif command == 'g':
                    self.vc.printGraph()
                    sys.stdout.flush()

            except vertex_cover.VC_Exception, e:
                self.printError( str(e) + ' INPUT: ' + line.strip() )

            except Exception, e:
                self.printError('Invalid input. Unknown error.')
        # end of while loop =================

#############
# End of UI
#############

