#importing the Random library
from random import randint
import time
#Setting up my node class with all needed functions
class Node:
    def __init__(self,board,cost,children,parent):
        self.board = board
        self.cost = cost
        self.children = children
        self.parent = parent
        self.last = -5

#Simple random board generator, can randomly generate boards including
#impossible boards
def generateBoard():
    board = []
    pos = range(9)
    for i in range(9):
        random = randint(0,len(pos) - 1)
        board.append(pos[random])
        pos.remove(pos[random])
    return board
#First heuristic
#This heuristic calculates the total number of tiles that are out of their
#correct place and uses this total as h(n)
#takes in the current board as input
#returns a sum
def h1(board):
    sum = 0
    for i in range(len(board)):
        if board[i] != i:
            sum += 1
    return sum
#simple swap function
def swap(board,first,second):
    nboard = [x for x in board]
    temp = nboard[first]
    nboard[first] = nboard[second]
    nboard[second] = temp
    return nboard
#used in h2
#calculates the distance in rows the tile is from its correct position
#returns a value
def row(value):
    return value / 3
#used in h2
#calculates the distance in columns the tile is from its correct position
#returns a value
def column(value):
    return value % 3
#second heuristic
#This heuristic calculates the total distance that a particular tile is from
#its correct position and uses that value as h(n)
#takes in the current board as input
#returns a sum
def h2(board):
    sum = 0
    for cur in range(len(board)):
        rows = abs(row(cur) - row(board[cur]))
        col = abs(column(cur) - column(board[cur]))
        sum += rows + col
    return sum
#The inversion function
#calculates the number of inversion
#if the number is odd returns FALSE
#if the number is even it will return TRUE
def inversion(board):
    sum = 0
    nboard = [x for x in board]
    nboard.remove(0)
    for i in range(len(nboard)):
        for j in range(i + 1,len(nboard)):
            if nboard[i] > nboard[j]:
                sum += 1
    if sum % 2 == 1:
        return False
    return True
#function simply returns the index of where the zero tiles is
#zero tile represents the empty tile
#returns an index value
def zIndex(board):
    return board.index(0)
#this function is performing the actual tile movement
#returns the board after the move has been completed
#takes in the board and the move it should perform
def move(board, distance):
    return swap(board,zIndex(board),zIndex(board) + distance)


#Checks to see if the current state of the board is the goal state
#if yes returns true
#else it will return false
def isGoal(board):
    for i in range(len(board)):
        if board[i] != i:
            return False
    return True
#This function generates all of the possible moves based on the current
#state of the board
#will store of of these possible moves into a list => possible
#returns the list
def posStates(node):
    board = node.board
    last = node.last
    possible = []
    zero = zIndex(board)
    if (zero / 3 != 0 and last != 3):
        possible.append(-3)
    if (zero % 3 != 0 and last != 1):
        possible.append(-1)
    if (zero % 3 != 2 and last != -1):
        possible.append(1)
    if (zero / 3 != 2 and last != -3):
        possible.append(3)
    return possible

def leaves(root):
    leaves = []
    def get_leaf(node):
        if node is not None:
            if len(node.children) == 0:
                leaves.append(node)
            for n in node.children:
                get_leaf(n)
    get_leaf(root)
    return leaves
#This function returns the lowest cost
def lowest(ints):
    low = 0
    for val in range(1,len(ints)):
        if ints[val] < ints[low]:
            low = val
    return low
#passes in frontier
#Then calls lowest function
#returns a list of the lowest costs
def lowestCost(leave):
    costs = []
    for leaf in leave:
        costs.append(leaf.cost)
    return leave[lowest(costs)]
#
#This function checks if the children are actually valid moves
#returns true if it is a valid move
#otherwise will return false if it is not a valid move
def isValid(root):
    leave = leaves(root)
    for leaf in leave:
        if inversion(leaf.board):
            return True
    return False
#removes a child node
#no return
def removeLeaf(node):
    parent = node.parent
    (parent.children).remove(node)
    if len(parent.children) == 0:
        if parent.parent == None:
            print "There is no Solution"
            exit()
        removeLeaf(parent)
#simply retrieves the depth of the current node calling the function
#recursive call
#returns a value
#takes in a node as input arg
def depth(node):
    if node.parent == None:
        return 0
    else:
        return 1 + depth(node.parent)

def equals(board1,board2):
    for x in range(len(board1)):
        if (board1[x] != board2[x]):
            return False
    return True
#python only writes to file in a string format, transfers board to string
#receives a board as input args
#returns string version
def stringVal(board):
    val = ""
    for str in board:
        val += "%d" % str
    return val
#checking to see if board is contained within hash table
def contains(table, board):
    stringBoard = stringVal(board)
    try:
        table[hash(stringBoard)]
        return True
    except KeyError as e:
        return False
#main portion of the printing to file
#utilizes the print board function
def printTree(goal,file):
    if goal.parent == None:
        printBoard(goal,file)
        file.write("%d\n" % goal.cost)
    else:
        printTree(goal.parent,file)
        printBoard(goal,file)
        file.write("%d\n" % goal.cost)
#prints the board in the proper format
#writes to a text file
#OUTPUT
#takes in the goal board and also takes in the file as input args
#no return
def printBoard(goal,file):
    for x in range(0,len(goal.board),3):
        file.write("| %d %d %d |\n" % ((goal.board)[x], (goal.board)[x+1], (goal.board)[x+2]))

#retrieves the lowest depth board that has be executed properly
#takes in the two arrays that are storing the alg analysis data
#grabs the lowest for both
#returns the lowest
def lowDepth(H1,H2):
    low = H1[0][0]
    for x in range(len(H1)):
        if H1[x][0] < low:
            low = H1[x][0]
    for x in range(len(H2)):
        if H2[x][0] < low:
            low = H2[x][0]
    return low
#retrieves the highest depth board that has be executed properly
#takes in the two arrays that are storing the alg analysis data
#grabs the highest for both
#returns the highest
def highDepth(H1,H2):
    high = H1[0][0]
    for x in range(len(H1)):
        if H1[x][0] > high:
            high = H1[x][0]
    for x in range(len(H2)):
        if H2[x][0] > high:
            high = H2[x][0]
    return high


#Setting up the text file for writing only
#will overwrite what ever is currently in text file
testfile = open('test1.txt', 'w')
#set the counter variable
#represents the board number that is currently being executed
inputs = 1
#H1 array holds all necessary data that is need for algorithm analysis
H1 = []
#H1 array holds all necessary data that is need for algorithm analysis
H2 = []
#inputs are tested to 200
#100 per heuristic
while( inputs <= 200):
    #beginning timer
    start = time.time()
    #resetting total number of steps after each iteration
    totalsteps = 0
    if inputs % 2 == 1:
        board = generateBoard()
    table = {}
    #holds all of the frontier nodes for processing
    leaves = []
    #Test to see if the current board is an inversion
    #if returned true will moves to else statement, not incremnet the counter
    #and repeat the board number
    if inversion(board):
        #initializing the node class
        root = Node(board,0,[],None)
        leaves.append(root)
        x = 1
        #running A* and all functions associated with it
        #breaks when goal is reached
        while(True):
            current = lowestCost(leaves)
            totalsteps += 1
            #check if the current state is the goal state
            if isGoal(current.board):
                #the if else block allows for the same board to be ran through
                #2 different heuristics
                #also stops the time and appends the data to the proper data
                #holding array set
                if inputs % 2 == 1:
                    testfile.write("H1 Algorithm\n")
                    H1.append([depth(current),totalsteps,time.time() - start])
                else:
                    testfile.write("H2 Algorithm\n")
                    H2.append([depth(current),totalsteps,time.time() - start])
                #increment the counter
                inputs+=1
                break
            #if the current state of the board is not the goal state
            #execute
            else:
                #generate possible moves
                moves = posStates(current)
                children = []
                #generate all the possible boards given the possible moves
                #to be passed into the respective heuristic
                for movement in moves:
                    newboard = move(current.board,movement)
                    #verify that the move is not an inversion
                    if inversion(newboard) and not contains(table,newboard):
                        node = Node(newboard,0,[],current)
                        #the if else block allows for the same board to be ran through
                        #2 different heuristics
                        #also stops the time and appends the data to the proper data
                        #holding array set
                        if inputs % 2 == 1:
                            node.cost = current.cost + h1(node.board)
                        else:
                            node.cost = current.cost + h2(node.board)
                        node.last = movement
                        children.append(node)
                        leaves.append(node)
                        #using hashing in order to increase speed and performance
                        table[hash(stringVal(node.board))] = node.board
                current.children = children
                leaves.remove(current)
                x += 1

    else:
        print "The initial board is impossible to solve:\n"
        print board
        print "board number: %d" % inputs

#This area is going through the data filled arrays
#Sorting the data and assembling the averages
#Then prints the data out after all iterations have been completed
high = highDepth(H1,H2)
low = lowDepth(H1,H2)
#New arrays that are being filled with averaged data and processed
H1avg = []
H2avg = []

for depth in range(low,high + 1):
    averagesteps1 = 0
    averagesteps2 = 0
    averagetime1 = 0
    averagetime2 = 0
    number1 = 0
    number2 = 0
    #running through the h1 set of data points
    for pos in H1:
        if pos[0] == depth:
            #accumulating averages
            averagesteps1 += pos[1]
            averagetime1 += pos[2]
            number1 += 1
    #
    for pos in H2:
        if pos[0] == depth:
            averagesteps2 += pos[1]
            averagetime2 += pos[2]
            number2 += 1
    #making sure that that its valid and placing the data within
    # the proper area of the data
    if number1 > 0:
        H1avg.append([depth,averagesteps1/number1,averagetime1/number1])
    if number2 > 0:
        H2avg.append([depth,averagesteps2/number2,averagetime2/number2])
#outputting the data arrays now holding
#depth
#average number of steps per that depth
#average amount of time to execute the algorithms
#with that amount of depth
print H1avg
print H2avg
#close the file
testfile.close()