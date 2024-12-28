from typing import Callable, Optional
from random import choice, randint, shuffle, choices, random

##################################################
################### STATE TYPE ###################
##################################################

State = list[int] #list of 8 integers

def valid_state(state : State) -> bool:
    '''Checks if `state` is a list of nonnegative integers below 8'''
    if not isinstance(state, list):
        return False
    return (
        len(set(state)) == len(state) and
        all(0 <= num < 8 and isinstance(num, int) for num in state)
    )

def valid_population(states : list[State]) -> bool:
    '''Checks if every element of `states` is a valid state'''
    return all(valid_state(state) for state in states)

##################################################
########## INITIALIZATION OF POPULATION ##########
##################################################

def random_state() -> State:
    '''Returns a randomly initizalized state'''
    state = list(range(8))
    shuffle(state)
    return state

def random_population(population_size : int) -> list[State]:
    '''Returns a randomly initizalized population (list of states)'''
    return [ random_state() for _ in range(population_size) ]

##################################################
################ FITNESS FUNCTION ################
##################################################

def fitness(state : State) -> int:
    '''Returns the number of nonattacking pairs of queens'''
    assert valid_state(state)
    attacks = 0
    for x_a, y_a in enumerate(state):
        for x_b, y_b in enumerate(state):
            if x_b < x_a+1: continue
            attacks += int(x_a+y_a == x_b+y_b) + int(y_a-x_a == y_b-x_b)
    return sum(range(8)) - attacks

def is_solution(state : State) -> bool:
    '''
    Returns if the provided state is a goal state.
    Returns if the number of nonatacking pairs of queens = all pairs of queens
    '''
    return fitness(state) == sum(range(8))

def contains_solution(states : list[State]) -> Optional[State]:
    '''Returns the solution state (for visualization) if there is any in the population'''
    for state in states:
        if is_solution(state):
            return state
    return None

##################################################
############### PRINTING FUNCTIONS ###############
##################################################

def print_state(state : State):
    queens = list(enumerate(state))
    for i in range(8):
        for j in range(8):
            print('|', end='')
            if (i,j) in queens:
                print('Q', end='')
            else:
                print('_', end='')
        print('|')


def print_population(states : list[State], f : Callable[[State],int] = fitness) -> None:
    for state in states:
        assert valid_state(state)
        print(state, '-->', f(state))
    print('#'*31)

##################################################
################### SELECTION ####################
##################################################

def selection(states : list[State], min_val : int, 
                f : Callable[[State],int] = fitness, 
                oversampling : bool = True
             ) -> list[State]:
    '''Applies regular selection on population'''
    preserved_states = [ state for state in states if f(state) > min_val ]
    if oversampling:
        while len(preserved_states) < len(states):
            preserved_states.append(choice(preserved_states))
    assert valid_population(preserved_states)
    return preserved_states

def selection_roulette(states : list[State], 
                        f : Callable[[State],int] = fitness
                      ) -> list[State]:
    '''Applies roulette wheel selection on population'''
    pass #TODO
    # Idea: Check lecture 7 (evolution) slide 14
    #       Use random.choices (and the weights and k optional argument)
    #           to choose k "good" states randomly
    #       The weights should be the deduced using the fitness function
    #       Finally, return the preserved states.
    fitness_values = [f(state) for state in states] 
    total_fitness = sum(fitness_values)
    probabilities = [fit / total_fitness for fit in fitness_values]
    selected_states = choices(states, weights=probabilities, k=len(states))
    assert valid_population(selected_states)
    return selected_states

##################################################
################# RECOMBINATION ##################
##################################################

def recombination(states : list[State]) -> list[State]:
    '''Applies recombination step on population'''
    assert isinstance(states,list)
    new_states : list[State] = []
    for i in range(0,len(states)-1,2):
        new_states += recombine(states[i], states[i+1])
    assert valid_population(states)
    return new_states

def recombine(state_a : State, state_b : State) -> tuple[State, State]:
    '''Applies recombination step on two states'''
    pass #TODO
    # Idea: Check lecture 7 (evolution) slide 12
    #       choose 2 random indexes for division barriers, and then 
    #       perform the recombination
    assert valid_state(state_a) and valid_state(state_b)
    cut1, cut2 = sorted([randint(0, 7) for _ in range(2)])
    child1 = state_a[:cut1] + state_b[cut1:cut2] + state_a[cut2:]
    child2 = state_b[:cut1] + state_a[cut1:cut2] + state_b[cut2:]
    return child1, child2
##################################################
##################### REPAIR #####################
##################################################

def repair(states : list[State]) -> list[State]:
    '''Applies repair step on population'''
    new_states : list[State] = []
    for i in range(0,len(states)-1,2):
        new_states += repair_states(states[i], states[i+1])
    assert valid_population(new_states)
    return new_states

def repair_states(state_a : State, state_b : State) -> tuple[State,State]:
    '''Applies repair step on two states'''
    state_a_, state_b_ = state_a.copy(), state_b.copy()
    pass #TODO
    # Idea: Check for each element in state_a_ if it is contained twice.
    #       If so, find a good substitute for it in state_b_
    def repair_single(state, partner):
        elements = set(range(8))
        missing = elements - set(state)
        duplicates = [x for x in state if state.count(x) > 1]
        for i, val in enumerate(state):
            if state.count(val) > 1:
                state[i] = missing.pop()
        return state

    return repair_single(state_a.copy(), state_b.copy()), repair_single(state_b.copy(), state_a.copy())

##################################################
#################### MUTATION ####################
##################################################

def mutation(states : list[State], chance : float) -> list[State]:
    '''Applies mutation step on population'''
    new_population = [mutate(state, chance) for state in states]
    assert valid_population(new_population)
    return new_population

def mutate(state : State, chance : float) -> State:
    '''Applies mutation step on one state'''
    mutated_state : State = state.copy()
    pass #TODO
    # Idea: Pick 2 random indexes and swap the elements on those positions
    #       Do that only with the given chance! (Suggestion: use random.random())
    mutated_state = state.copy()
    if random() < chance:
        idx1, idx2 = randint(0, 7), randint(0, 7)
        mutated_state[idx1], mutated_state[idx2] = mutated_state[idx2], mutated_state[idx1]
    return mutated_state

##################################################
################## REPLACEMENT ###################
##################################################

def replacement(original : list[State], evolved : list[State], 
                k : int,
                f : Callable[[State],int] = fitness
             ) -> list[State]:
    '''Applies replacement step based on original and evolved populations'''
    return sorted(original,key=f)[k:] + sorted(evolved,key=f)[-k:]

##################################################
################### SIMULATION ###################
##################################################

def simulate(starting_state : list[State], 
             num_cycles : int,
             mutation_prob : float,
             use_roulette : bool,
             min_fitness : Optional[int]) -> None:
    current_state = starting_state.copy()
    for i in range(num_cycles):
        evolved_state = (
            selection_roulette(current_state)
            if use_roulette else
            selection(current_state, min_fitness)
        )
        shuffle(evolved_state)
        evolved_state = recombination(evolved_state)
        evolved_state = repair(evolved_state)
        evolved_state = mutation(evolved_state, mutation_prob)
        current_state = replacement(current_state,evolved_state,len(current_state)//2)
        print_population(current_state)
        solution = contains_solution(current_state)
        if solution:
            print(f'solution found after {i+1} steps')
            print_state(solution)
            break
    else:
        print('No solution was found')

def main():
    pop_size = input('Population size:')
    num_iter = input('Number of iterations:')
    mut_prob = input('Mutation probability:')
    roulette = input('Use roulette selection [Y/n]:')
    
    pop_size = int(pop_size)   if pop_size else 4
    num_iter = int(num_iter)   if num_iter else 100
    mut_prob = float(mut_prob) if mut_prob else 0.75
    assert 0 <= mut_prob <= 1
    roulette = True if roulette in ['Y','y',''] else False
    
    if not roulette:
        min_fit = input('Minimum fitness value for selection:')
        min_fit = int(min_fit) if min_fit else 21
    else:
        min_fit = None
    
    simulate(random_population(pop_size),num_iter,mut_prob,roulette,min_fit)

if __name__ == "__main__":
    main()