#importing library's
from random import randint
import math
import time

#global variable for if steepest hill climbing alg has made a move or not
nMove = True
#This function generates a 1D array
#board[i] where i represents the column of the board
#borad[i] = r where the value r stored at board[i] is what row that queen is at
#returns a board where each queen gets a unique column but not nessecarily a unique row
def generateQueenBoard():
    board = []
    pos = range(15)
    for i in range(15):
        random = randint(0, len(pos) - 1)
        board.append(pos[random])
        #pos.remove(pos[random])
    return board
#Thie function is the heuristic function
#returns a total amount of queens attacking eachother
#checks for if attack queen in same row
#as well as if attacking queen in a diagonal
#uses the offset to check both upper diagonal and lower diagonals
#returns a integer h
def get_h_cost(board):
  h = 0
  for i in range(len(board)):  # Check every column we haven't already checked
    for j in range(i + 1, len(board)):  # Queens are in the same row
      if board[i] == board[j]:
        h += 1
      offset = j - i  # Get the difference between the current column
      #checking diagonals
      if board[i] == board[j] - offset or board[i] == board[j] + offset:
        h += 1
  return h
#this fuction makes one move using the steepest hillclimbing algorithm
#checks for the move with the best heuristic
#if no move has a better heuristic than that of the current board
#no move is made
def makeMoveSteepest(board):
  #moves dictionary
  moves = {}
  #iterating through board, generating
  #possible moves
  for col in range(len(board)):
    best_move = board[col]

    for row in range(len(board)): #this loop moves the selected queen up and down its column
      if board[col] == row:
        #We don't need to evaluate the current
        #position, we already know the h-value
        continue

      bcopy = list(board)
      #Move the queen to the new row
      bcopy[col] = row
      moves[(col,row)] = get_h_cost(bcopy)

  best_moves = []
  best_heuristic = get_h_cost(board)
  for c,v in moves.iteritems():
    if v < best_heuristic:   #if its lower, update the heuristic to beat
      best_heuristic = v

  for c,v in moves.iteritems():
    if v == best_heuristic:
      best_moves.append(c)
      break;

  #Pick best move
  if len(best_moves) > 0:
    nMove = True
    col = best_moves[0][0]
    row = best_moves[0][1]
    #make move
    board[col] = row
    return board
  #returns the original board if no move is better that current heuristic
  #set newmove (nMove) to False
  nMove = False
  return board

#need libray for random object
#
import random
def moveAnnealing(board,startingH,temp):
  copy = list(board)
  foundMove = False

  while not foundMove:
    copy = list(board)
    new_row = randint(0,len(board)-1)
    new_col = randint(0,len(board)-1)
    copy[new_col] = new_row
    newH = get_h_cost(copy)
    if newH < startingH:
      foundMove = True
    else:
      #How bad was the choice?
      delta_e = startingH - newH
      #Probability can never exceed 1
      accept_probability = min(1,math.exp(delta_e/temp))
      # if true, means move is within acceptable range, allows move
      # found move = True
      # returns the updated board
      foundMove = random.random() <= accept_probability

  return copy

#
#   BEGIN MAIN
#   first while loop runs steepest hill climbing
#   second while loop runs simulated annealing
#
steepestTime = []
moves = 0
winner = 0
loser = 0
bNum = 0
grandtotal = 0
totalmoves = 0
while(bNum < 200): #run with 200 randomly generated boards
    moves = 0
    #generate the board
    board = generateQueenBoard()
    #print out the initial state of the board
    print "\nINITIAL BOARD #%d" % (bNum+1)
    print board
    #reset variables
    x = 0
    totalcost = 0
    attackingQs = 0
    #begin the timer
    start = time.time()
    #by Dr. Tang's advice, I limited the amount of iteratations in order to prevent looping
    #looping can happen where a move is made however it ends up remaking the same move
    #after it makes another move
    while(x != 20):
        #make move
        board = makeMoveSteepest(board)
        #get heuristic of new move
        attackingQs = get_h_cost(board)
        totalcost += attackingQs
        moves +=1
        #if there are no attacking queens, found a solution
        if not attackingQs:
            #stop timer
            steepestTime.append(time.time() - start)
            print "SOLVED BOARD #%d" % (bNum+1)
            print board
            totalmoves += moves
            winner += 1
            solved = True
            bNum += 1
            grandtotal += totalcost
            break
        #if there was no move made, since all possible moves are generated
        #no more use in running, no solution can be found
        if not nMove:
            break
        else:
            solved = False
            x += 1
    #if no solution found in given amount of iterations, most likely the board is caught in a local
    #local maxima or minima, can exit
    if not solved:
        loser += 1
        print "No solution found for board #%d \n continuing..." % (bNum+1)
        steepestTime.append(time.time() - start)
        grandtotal += totalcost
        bNum += 1
avgTime = 0
n = 0
for t in range(len(steepestTime)):
    avgTime += steepestTime[t]
    n += 1
AVGtime = avgTime/n
avgMoves = totalmoves/winner
avg = grandtotal/200
print "\n\n\n***RESULTS STEEPEST HILL CLIMBING***\n"
print "Number of solved boards %d" % winner
print "\nNumber of unsolved boards %d" % loser
print "Average cost %d" % avg
print "Average moves %d" % avgMoves
print "Average Time: %f" % AVGtime
#
#
#BEGIN SIMULATED ANNEALING LOOP
#
#
#RESET ALL REUSED VARIABLES
annealTime = []
moves = 0
winner = 0
loser = 0
bNum = 0
grandtotal = 0
totalmoves = 0
while(bNum < 200): #run 200 randomly generated boards
    moves = 0
    board = generateQueenBoard()
    print "\nINITIAL BOARD #%d" % (bNum+1)
    print board
    x = 0
    totalcost = 0
    steps = 0
    #set initial temperature to be high in order to give greater ability to make bad moves
    temp = len(board)**2
    #rate to cut down on bad decisions (the closer to 1, the slower the rate)
    #want as slow as a rate as possible, gives best ability to find solution
    anneal_rate = 0.99
    attackingQs = get_h_cost(board)
    start = time.time()
    while attackingQs > 0:
        board = moveAnnealing(board,attackingQs,temp)
        attackingQs = get_h_cost(board)
        #update total cost thus far
        totalcost += attackingQs
        #Make sure temp doesn't get impossibly low
        new_temp = max(temp * anneal_rate,0.01)
        temp = new_temp
        moves += 1
        steps += 1
        #break out if stuck
        if steps >= 7000:
            break
        # found solution
        if attackingQs == 0:
            annealTime.append(time.time() - start)
            print "SOLVED BOARD #%d" % (bNum+1)
            print board
            winner += 1
            solved = True
            bNum += 1
            grandtotal += totalcost
            totalmoves += moves
            break
        else:
            solved = False
    if not solved:
        annealTime.append(time.time() - start)
        loser += 1
        print "No solution found in allowed number of iterations"
        grandtotal += totalcost
        bNum += 1


avgTime = 0
n = 0
for t in range(len(annealTime)):
    avgTime += annealTime[t]
    n += 1
AVGtime = avgTime/n
avgMoves = totalmoves/winner
avg = grandtotal/200
print "\n\n\n***RESULTS SIMULATED ANNEALING***\n"
print "Number of solved boards %d" % winner
print "\nNumber of unsolved boards %d" % loser
print "Average cost %d" % avg
print "Average moves %d" % avgMoves
print "Average Time: %f" % AVGtime
