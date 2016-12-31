import random
import time
import itertools

countCompositionsCache = {}


# Adapted from http://stackoverflow.com/a/2163753
def countCompositions(elems, sum):
    if sum == 0:
        return 1
    elif (elems, sum) in countCompositionsCache:
        return countCompositionsCache[elems, sum]
    
    numComps = 0
    
    for e in elems:
        if e <= sum:
            numComps = numComps + countCompositions(elems, sum - e)

    countCompositionsCache[elems, sum] = numComps
    
    return numComps

def countCompositionsPredicate(elems, sum, predicate, seq = []):
    if not predicate(seq, sum):
        return 0
    if sum == 0:
        return 1
    
    numComps = 0
    
    for e in elems:
        newSeq = seq[:]
        newSeq.append(e)
        
        if e <= sum:
            numComps = numComps + countCompositionsPredicate(elems, sum - e, predicate, newSeq)
            
    return numComps


# Adapted from http://stackoverflow.com/a/2163753
def randomComposition(elems, sum):
    numComps = countCompositions(elems, sum)
    selected = random.randrange(numComps)
    
    comp = []
    
    while sum != 0:
        for e in elems:
            numSubComps = countCompositions(elems, sum - e)
            
            if selected < numSubComps:
                break
            
            selected = selected - numSubComps
            
        comp.append(e)
        sum = sum - e
        
    return comp

def randomCompositionPredicate(elems, sum, predicate):
    numComps = countCompositionsPredicate(elems, sum, predicate)
    selected = random.randrange(numComps)
    
    comp = []
    
    while sum != 0:
        for e in elems:
            newSeq = comp[:]
            newSeq.append(e)
            
            numSubComps = countCompositionsPredicate(elems, sum - e, predicate, newSeq)
            
            if selected < numSubComps:
                break
            
            selected = selected - numSubComps
            
        comp.append(e)
        sum = sum - e
        
    return comp

def countmaxconsecutive(list, elem):
    maxcount = 0
    for e, group in itertools.groupby(list):
        if e == elem:
            maxcount = max(maxcount, sum(1 for _ in group))
    return maxcount

def countminconsecutive(list, elem):
    mincount = len(list)+1
    for e, group in itertools.groupby(list):
        if e == elem:
            mincount = min(mincount, sum(1 for _ in group))
    return mincount
