import sys
import math
import ece650_a1_ui as ui


def main():
    ### YOUR MAIN CODE GOES HERE
    a1ece = ui.UI()
    if len(sys.argv) == 2 and sys.argv[1]=='-v':    a1ece.debugOn()
    a1ece.run()

    # return exit code 0 on successful termination
    sys.exit(0)

if __name__ == '__main__':
    main()

"""
REFERENCES:
    https://stackoverflow.com/
    https://github.com/eceuwaterloo/ece650-py/blob/master/intersect.py
    https://www.codecademy.com/learn/learn-python
    https://www.cs.hmc.edu/ACM/lectures/intersections.html
    https://docs.python.org/2/tutorial/index.html
"""
