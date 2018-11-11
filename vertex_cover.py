import math

#====================
class VC_Exception(Exception):

    def __init__(self,message):
        Exception.__init__(self, message)

#====================
class Point(object):
    def __init__ (self, x, y):
        self.x = float(x)
        self.y = float(y)
    def __str__ (self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'
    def __lt__(pt1, pt2):
        if pt1.x < pt2.x:
            return True
        elif pt1.x > pt2.x:
            return False
        elif pt1.y < pt2.y:
            return True
        else:
            return False

    def __gt__(pt1, pt2):
        return pt2 < pt1

    def __eq__(pt1, pt2):
        return (pt1.x == pt2.x and pt1.y == pt2.y)

#====================
class Line(object):
    def __init__ (self, pt1, pt2):
        self.vertex  = {}
        if pt1 > pt2:
            self.start = pt1
            self.end = pt2
        else:
            self.start = pt2
            self.end = pt1

    def clearVertex(self):
        self.vertex = {}

    def addVertex(self, vertex):
        if self.vertex:
            self.vertex[ str(vertex) ] = vertex
        else:
            self.vertex[ str(self.start) ] = self.start
            self.vertex[ str(vertex) ] = vertex
            self.vertex[ str(self.end) ] = self.end

    def printEdges(self, vertex_index):
        output = []
        if self.vertex:
            edge_start = None
            sorted_vertex_keys = sorted(self.vertex.values())
            for edge_end in sorted_vertex_keys:
                if edge_start:
                    output.append('<' + str(vertex_index[str(edge_start)]) + "," + str(vertex_index[str(edge_end)]) + ">")

                edge_start = edge_end

        return output

    def __str__(self):
        return str(self.start) + '-->' + str(self.end) + '\n\tV = ' + str(sorted(self.vertex.values()))

#====================
class Street(object):
    def __init__ (self, name, coordinates):
        self.name = name
        self.lines = []

        prev = None
        for next in coordinates:
            if prev:
                self.lines.append( Line( Point( prev[0], prev[1] ), Point( next[0], next[1]) ) )
                prev = next
            else:
                prev = next

    def __str__(self):
        str_output = "'" + self.name + "':\n"
        for line in self.lines:
            str_output = str_output + str(line) + "\n"
        return str_output

    def clearVertex(self):
        for line in self.lines:
            line.clearVertex()

    def printEdges(self, vertex_index):
        output = []
        for line in self.lines:
            output.extend(line.printEdges(vertex_index))

        return output

#====================
class VertexCover(object):
    #----------------------
    def __init__(self):
        self.debug = False
        self.streets = {}
        self.vertex= {}

    #----------------------
    def addStreet(self, street_name, coordinates):
        street_name = street_name.strip()
        if street_name.lower() in self.streets:
            raise VC_Exception("Street name '" + street_name  + "' already exists.")
        else:
            self.streets[street_name.lower()] = Street( street_name, coordinates )

    #----------------------
    def removeStreet(self, street_name):
        street_name = street_name.strip()
        if street_name.lower() in self.streets:
            del self.streets[street_name.lower()]
        else:
            raise VC_Exception("Street name '" + street_name  + "' does not  exists.")

    #----------------------
    def changeStreet(self, street_name, coordinates):
        self.removeStreet(street_name)
        self.addStreet(street_name, coordinates)

    #----------------------
    def getVertexIndex(self):
        vertex_keys = sorted(self.vertex.keys())
        vertex_index = {}
        i = 0
        for index in vertex_keys:
            vertex_index[ index ] = i
            i += 1

        return vertex_index

    #----------------------
    def printVertex(self, vertex_index):
        print "V " + str(len(vertex_index))

        if self.debug:
            print "{"
            for key,index in sorted(vertex_index.items()):
                print "  " + str(index) + ":\t" + str(self.vertex[key])

            print  "}"

    #----------------------
    def printEdges(self, vertex_index):
        edges = []
        exists = {}
        output = "E {"

        for key,street in self.streets.items():
            edges.extend( street.printEdges(vertex_index) )

        count = len( edges )
        for index in range(0, count):
            if edges[index] in exists:
                continue
            elif index == count - 1:
                output = output + edges[index].strip()
            else:
                output = output + edges[index].strip() + ","

            exists[ edges[index] ] = index

        output = output + "}"
        print output

    #----------------------
    def clearVertex(self):
        self.vertex = {}
        for key,street in self.streets.items():
            street.clearVertex()

    #----------------------
    def generateGraph(self):
        self.clearVertex()

        if self.debug:
            print "===================================================="

        keys = self.streets.keys()


        i=0; j=1; count=len(keys)
        while i < count-1 and j < count:
            key1 = keys[i]
            key2 = keys[j]
            st1 = self.streets[key1]
            st2 = self.streets[key2]
            for ln1 in range(0,len(st1.lines)):
                line1 = st1.lines[ln1]
                for ln2 in range(0,len(st2.lines)):
                    line2 = st2.lines[ln2]
                    if self.debug:
                        print st1.name + " / " + st2.name

                    intersect = self.getIntersection( line1, line2 )
                    if intersect:
                        self.streets[key1].lines[ln1].addVertex( intersect )
                        self.streets[key2].lines[ln2].addVertex( intersect )
                        self.vertex.update( self.streets[key1].lines[ln1].vertex )
                        self.vertex.update( self.streets[key2].lines[ln2].vertex )

            j = j + 1
            if j==count:
                i=i+1; j=i+1
        # while

    #----------------------
    def printGraph(self):
        self.generateGraph()

        vertex_index = self.getVertexIndex()
        self.printVertex(vertex_index)
        self.printEdges(vertex_index)

    #----------------------
    def getIntersection(self, line1, line2):

        DET_TOLERANCE = 0.00000001

        # the first line is S1p1 + r*(S1p2-pt1)
        # in component form:
        S1x1 = line1.start.x;   S1y1 = line1.start.y
        S1x2 = line1.end.x;     S1y2 = line1.end.y
        S1dx = S1x2 - S1x1;     S1dy = S1y2 - S1y1

        # the second line is S2p1 + s*(S2p2-ptA)
        S2x1 = line2.start.x;   S2y1 = line2.start.y;
        S2x2 = line2.end.x;     S2y2 = line2.end.y
        S2dx = S2x2 - S2x1;  S2dy = S2y2 - S2y1;

        # we need to find the (typically unique) values of r and s
        # that will satisfy
        #
        # (S1x1, S1y1) + r(S1dx, S1dy) = (x, y) + s(S2dx, S2dy)
        #
        # which is the same as
        #
        #    [ S1dx - S2dx ][ r ] = [ S2x1 - S1x1 ]
        #    [ S1dy - S2dy ][ s ] = [ S2y1 - S1y1 ]
        #
        # whose solution is
        #
        #    [ r ] = _1_  [ -S2dy  S2dx ] [ S2x1 - S1x1 ]
        #    [ s ] = DET  [ -S1dy  S1dx ] [ S2y1 - S1y1 ]
        #
        # where DET = (-S1dx * S2dy + S1dy * S2dx)
        #
        # if DET is too small, they're parallel
        #
        DET = (-S1dx * S2dy + S1dy * S2dx)
            
        if math.fabs(DET) < DET_TOLERANCE:
            return None

        # now, the determinant should be OK
        DETinv = 1.0/DET


        #===================

        # find the scalar amount along the "self" segment
        r = DETinv * (-S2dy * (S2x1 - S1x1) + S2dx * (S2y1 - S1y1))

        # find the scalar amount along the input line
        s = DETinv * (-S1dy * (S2x1 - S1x1) + S1dx * (S2y1 - S1y1))

        if self.debug: print "r = ", r, " s = ", s

        # return the average of the two descriptions
        xi = round( (S1x1 + r*S1dx + S2x1 + s * S2dx)/2.0, 2)
        yi = round( (S1y1 + r*S1dy + S2y1 + s * S2dy)/2.0, 2)

        if min(S1x1,S1x2) <= xi and xi <= max(S1x1,S1x2) and min(S1y1,S1y2) <= yi and yi <= max(S1y1,S1y2) and min(S2x1,S2x2) <= xi and xi <= max(S2x1,S2x2) and min(S2y1,S2y2) <= yi and yi <= max(S2y1,S2y2):
            return Point(xi,yi)
        else:
            return None
