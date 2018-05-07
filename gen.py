import random; 
import sys;  

print sys.argv[1]

for i in range(int(sys.argv[1])):
    print "(" + str(random.uniform(- (2 << 15), 2 << 15 )) + "," + str(random.uniform(- (2 << 15), 2 << 15 )) + ")"