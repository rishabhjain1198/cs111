#!/usr/bin/env python
"""
NAME: RISHABH JAIN
EMAIL: rishabhjain@ucla.edu
ID: 604817863

"""

#NEED TO TAKE CARE OF RESERVED BLOCKS CONFUSION

from __future__ import print_function
import sys

problemFound = 0

reservedBlocks = []
#reservedBlocks = ['1', '3', '4', '5', '6', '7',]

parentDict = {}

#SEEMS LIKE RESERVED BLOCKS ARE 1 TO 7, excluding 2

def duplicatePrint(indirection, block, inode, offset):
    if indirection == '0':
        print("DUPLICATE BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)
    elif indirection == '1':
        print("DUPLICATE INDIRECT BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)
    elif indirection == '2':
        print("DUPLICATE DOUBLE INDIRECT BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)
    else:
        print("DUPLICATE TRIPLE INDIRECT BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)

def invalidPrint(indirection, block, inode, offset):
    if indirection == '0':
        print("INVALID BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)
    elif indirection == '1':
        print("INVALID INDIRECT BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)
    elif indirection == '2':
        print("INVALID DOUBLE INDIRECT BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)
    elif indirection == '3':
        print("INVALID TRIPLE INDIRECT BLOCK " + block + " IN INODE "+ inode + " AT OFFSET " + offset)

def stderrprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

class dirEnt:
    linkCount = 0
    name = ''
    parentInode = 0
    def __init__(self, linkCount, name, parentInode):
        self.name = name
        self.linkCount = linkCount
        self.parentInode = parentInode
    def __repr__(self):
#        return 'NAME: ' + self.name + ' LINKCOUNT: ' + self.linkCount + ' PARENTINODE: ' + self.parentInode
        return 'LINKCOUNT ' + self.linkCount

class multiRef:
    inode = 0
    offset = 0
    printed = 0
    indirection = 0
    def __init__(self, inode, offset, indirection):
        self.inode = inode
        self.offset = offset
        self.printed = 0
        self.indirection = indirection
    def __repr__(self):
        return 'inode ' + self.inode + ', offset: ' + self.offset + ', indirection: ' + self.indirection
    

if(len(sys.argv) != 2):
    stderrprint("Invalid number of arguments!")
    exit(1)

with open(sys.argv[1], 'r') as openedFile:
    summaryData = openedFile.readlines()

summaryData = list(map(lambda s: s.strip(), summaryData))

# LOOK FOR SUPERBLOCK SUMMARY

for line in summaryData:
    lineData = line.split(',')
    firstWord = lineData[0]
    if firstWord == 'GROUP':
        reservedBlocks.append(lineData[-1])
        reservedBlocks.append(lineData[-2])
        reservedBlocks.append(lineData[-3])
        

for line in summaryData:
    lineData = line.split(',')
    firstWord = lineData[0]
    if firstWord == 'SUPERBLOCK':
        totalBlocks = int(lineData[1])
        totalInodes = int(lineData[2])
        firstNonReservedInode = int(lineData[7])

# DICTIONARY OF REFERENCED BLOCKS POINTING TO THE RESP. INODE
referencedBlocks = {}

for line in summaryData:
    lineData = line.split(',')
    firstWord = lineData[0]

    if firstWord == 'INODE':
        for i in range(12, 27):
            if int(lineData[i]) < 0 or int(lineData[i]) >= totalBlocks:
                # INVALID BLOCK FOUND
                problemFound = 2
                tableNumber = i-12
                if tableNumber < 12:
                    print("INVALID BLOCK " + lineData[i] + " IN INODE " + lineData[1] + " AT OFFSET " + str(i - 12))
                elif tableNumber == 12:
                    invalidPrint('1', lineData[i], lineData[1], str(tableNumber))
                elif tableNumber == 13:
                    invalidPrint('2', lineData[i], lineData[1], '268')

                elif tableNumber == 14:
                    invalidPrint('3', lineData[i], lineData[1], '65804')
            
            # DO SOME CHECK FOR RESERVED BLOCKS AS WELL
            if lineData[i] in reservedBlocks:
                 problemFound = 2
                 tableNumber = i -12
                 if tableNumber < 12:
                     print("RESERVED BLOCK "+ lineData[i] + " IN INODE " + lineData[1] + " AT OFFSET " + str(tableNumber))
                 elif tableNumber == 12:
                     print("RESERVED INDIRECT BLOCK "+ lineData[i] + " IN INODE " + lineData[1] + " AT OFFSET " + str(tableNumber))
                 elif tableNumber == 13:
                     print("RESERVED DOUBLE INDIRECT BLOCK "+ lineData[i] + " IN INODE " + lineData[1] + " AT OFFSET " + '268')
                 elif tableNumber == 14:
                     print("RESERVED TRIPLE INDIRECT BLOCK "+ lineData[i] + " IN INODE " + lineData[1] + " AT OFFSET " + '65804')



                 


            elif False:
                print("TAKE CARE OF RESERVED BLOCKS HERE")




            elif int(lineData[i]) != 0:
                temp = multiRef(lineData[1], str(i-12), '0')
                tableNumber = i -12
                if lineData[i] in referencedBlocks:
                    # DUPLICATE BLOCK FOUND
                    problemFound = 2
                    origBlock = referencedBlocks[lineData[i]]
            
                    if origBlock.printed == 0:
                        # PRINT THIS BLOCK DETAILS AND MAKE PRINTED 1
                        duplicatePrint(origBlock.indirection, lineData[i], origBlock.inode, origBlock.offset)
                        origBlock.printed = 1
                    if tableNumber < 12:
                        duplicatePrint('0', lineData[i], lineData[1], str(i-12))
                    elif tableNumber == 12:
                        duplicatePrint('1', lineData[i], lineData[1], str(tableNumber))
                    elif tableNumber == 13:
                        duplicatePrint('2', lineData[i], lineData[1], '268')
                    elif tableNumber == 14:
                        duplicatePrint('3', lineData[i], lineData[1], '65804')
                else:
                    referencedBlocks[lineData[i]] = temp

    if firstWord == 'INDIRECT':

        if int(lineData[5]) < 0 or int(lineData[5]) >= totalBlocks:
            # INVALID BLOCK FOUND
            problemFound = 2

            if lineData[2] == '1':
                print("INVALID INDIRECT BLOCK " + lineData[5] + " IN INODE " + lineData[1] + " AT OFFSET " + lineData[3])
            elif lineData[2] == '2':
                print("INVALID DOUBLE INDIRECT BLOCK " + lineData[5] + " IN INODE " + lineData[1] + " AT OFFSET " + lineData[3])
            else:
                print("INVALID TRIPLE INDIRECT BLOCK " + lineData[5] + " IN INODE " + lineData[1] + " AT OFFSET " + lineData[3])

        


        elif lineData[5] in reservedBlocks:
            problemFound = 2
            if lineData[2] == '1':
                print("RESERVED INDIRECT BLOCK " + lineData[5] + " IN INODE " + lineData[1] + " AT OFFSET " + lineData[3])
            elif lineData[2] == '2':
                print("RESERVED DOUBLE INDIRECT BLOCK " + lineData[5] + " IN INODE " + lineData[1] + " AT OFFSET " + lineData[3])
            elif lineData[2] == '3':
                print("RESERVED TRIPLE INDIRECT BLOCK " + lineData[5] + " IN INODE " + lineData[1] + " AT OFFSET " + lineData[3])



        elif lineData[5] != '0':
            temp = multiRef(lineData[1], lineData[3], lineData[2])
            if lineData[5] in referencedBlocks:
                # DUPLICATE BLOCK FOUND
                problemFound = 2
                origBlock = referencedBlocks[lineData[5]]
        
                if origBlock.printed == 0:
                    # PRINT THIS BLOCK DETAILS AND MAKE PRINTED 1
                    duplicatePrint(origBlock.indirection, lineData[5], origBlock.inode, origBlock.offset)
                    origBlock.printed = 1
                duplicatePrint(lineData[2], lineData[5], lineData[1], lineData[3])
            else:
                referencedBlocks[lineData[5]] = temp

freeBlocks = []

for line in summaryData:
    lineData = line.split(',')
    firstWord = lineData[0]

    if firstWord == 'BFREE':
        freeBlocks.append(lineData[1])


notFreeBlocks = []

for i in range(8, totalBlocks):
    if str(i) not in freeBlocks:
        notFreeBlocks.append(str(i))

for block in notFreeBlocks:
    if block not in referencedBlocks:
        print("UNREFERENCED BLOCK " + block)
        problemFound = 2

for block, inode in referencedBlocks.items():
    if block in freeBlocks:
        print("ALLOCATED BLOCK " + block + " ON FREELIST")
        problemFound = 2

#INODE STUFF

# INODE NUMBERS POINTING TO LINK COUNTS
referencedInodes = {}
freeInodes = []
notFreeInodes = []

for line in summaryData:
    lineData = line.split(',')
    firstWord = lineData[0]
    if firstWord == 'INODE':
        referencedInodes[lineData[1]] = lineData[6]


for line in summaryData:
    lineData = line.split(',')
    if lineData[0] == 'IFREE':
        freeInodes.append(lineData[1])

for i in range(firstNonReservedInode, totalInodes+1):
    if str(i) not in freeInodes:
        notFreeInodes.append(str(i))

for inode in notFreeInodes:
    if inode not in referencedInodes:
        print("UNALLOCATED INODE " + inode + " NOT ON FREELIST")
        problemFound = 2

for inode, links in referencedInodes.items():
    if inode in freeInodes:
        print("ALLOCATED INODE " + inode + " ON FREELIST")
        problemFound = 2

# ANOTHER DICTIONARY OF INODE NUMBERS POINTING TO LINK COUNTS
secondReferencedInodes = {}


for line in summaryData:
    lineData = line.split(',')
    if lineData[0] == 'DIRENT':

        inodeNumber = str(lineData[3])
        parentInode = str(lineData[1])

        if lineData[6] == "'.'":
            if parentInode != inodeNumber:
                print("DIRECTORY INODE " + parentInode + " NAME '.' LINK TO INODE " + inodeNumber + " SHOULD BE " + parentInode)
                problemFound = 2

        elif lineData[6] == "'..'":
            correctParent = parentDict.get(inodeNumber, str(2))
            if inodeNumber != correctParent:
                    print("DIRECTORY INODE " + parentInode + " NAME '..' LINK TO INODE " + inodeNumber + " SHOULD BE " + correctParent)
                    problemFound = 2

        else:
            parentDict[inodeNumber] = parentInode
                    


        if int(inodeNumber) < 0 or int(inodeNumber) > totalInodes:
            print("DIRECTORY INODE " + lineData[1] + " NAME " + lineData[6] + " INVALID INODE " + inodeNumber)
            problemFound = 2
        elif inodeNumber not in referencedInodes:
            print("DIRECTORY INODE " + lineData[1] + " NAME " + lineData[6] + " UNALLOCATED INODE " + inodeNumber)
            problemFound = 2

        else:
            linkCount = '1'
            if inodeNumber in secondReferencedInodes:
                linkCount = str(int(secondReferencedInodes[inodeNumber].linkCount) + 1)
            temp = dirEnt(linkCount, lineData[6], lineData[3])
            secondReferencedInodes[inodeNumber] = temp


for i in range(1, totalInodes + 1):
    inodeNumber = str(i)
    link = referencedInodes.get(inodeNumber, '0')
    linkCount = '0'
    if inodeNumber in secondReferencedInodes:
        linkCount = secondReferencedInodes[inodeNumber].linkCount
    if link != linkCount:
        print("INODE " + inodeNumber + " HAS " + linkCount + " LINKS BUT LINKCOUNT IS " + link)
        problemFound = 2


exit(problemFound)
