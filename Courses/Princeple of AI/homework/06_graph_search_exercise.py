import os
from functools import partial
from enum import Enum
import math
from dataclasses import dataclass
from typing import Any, Callable, Iterable, Type, Optional
from abc import ABC, abstractmethod

import FreeSimpleGUI as sg #type: ignore

from framework.board import Position, Board
from framework.gui import BoardGUI

LabyrinthFields = Enum("LabyrinthFields", "Empty Wall Cat Door Donut Bear")
Actions = Enum("Actions", "N W S E")
Directions = {
    Actions.N: Position(1, 0),
    Actions.E: Position(0, -1),
    Actions.S: Position(-1, 0),
    Actions.W: Position(0, 1),
}

RDirections = {v: k for k, v in Directions.items()}


@dataclass(frozen=True)
class State:
    """State for the simpler problem: getting to the door."""

    position: Position


@dataclass(frozen=True)
class EatDonutsState(State):
    """State for the problem where the cat has to eat all the donuts before getting
    to the door."""

    donuts: tuple[Position, ...]


class LabyrinthBoard(Board):
    """The playing board of the labyrinth game."""

    def __init__(self, path: str):
        """Initializes and loads a labyrinth from path."""
        self.load(path)

    def load(self, path: str) -> None:
        load_dict = {
            " ": LabyrinthFields.Empty,
            "W": LabyrinthFields.Wall,
            "C": LabyrinthFields.Cat,
            "D": LabyrinthFields.Door,
            "R": LabyrinthFields.Donut,
            "B": LabyrinthFields.Bear,
        }
        with open(path) as f:
            self.board = [[load_dict[ch] for ch in line.strip()] for line in f]
        for row in self.board:
            assert len(row) == len(self.board[0]), "The board should be rectangular!"
        self.m = len(self.board)
        self.n = len(self.board[0])


class LabyrinthGUI(BoardGUI):
    """The GUI of the labyrinth game."""

    def update_nodes(self, expanded_nodes: Iterable[State]) -> None:
        for i, state in enumerate(expanded_nodes):
            color = "#" + format(i % 200 + 50, "02x") * 3
            pos = state.position
            self.board_layout[pos.row][pos.col].Update(
                "", (color, color), image_paths[LabyrinthFields.Empty]
            )


class GameLogic:
    """This class implements the logic of the game (the actions)."""

    def __init__(self, board: Board):
        self.board = board
        self.donuts = 0
        self.cat_position = self.collect_positions(LabyrinthFields.Cat)[0]

    def collect_positions(self, field_type: LabyrinthFields) -> tuple[Position, ...]:
        """Collects the positions of the fields with type field_type."""
        return tuple(
            Position(i, j)
            for i, row in enumerate(self.board)
            for j, ft in enumerate(row)
            if ft == field_type
        )

    def do_action(self, action: Actions) -> None:
        """Executes an action in the game."""
        new_position = self.cat_position + Directions[action]
        new_field = self.board[new_position]
        if new_field == LabyrinthFields.Wall:
            return
        if self.board[self.cat_position] != LabyrinthFields.Door:
            self.board[self.cat_position] = LabyrinthFields.Empty
        if self.board[new_position] != LabyrinthFields.Door:
            self.board[new_position] = LabyrinthFields.Cat
        self.cat_position = new_position
        if new_field == LabyrinthFields.Donut:
            self.donuts += 1


class SearchProblem(ABC):
    """The abstract search problem."""

    state: State

    def __init__(self, game_logic: GameLogic):
        pass

    def start_state(self) -> State:
        return self.state

    @abstractmethod
    def target_positions(self, state: State) -> tuple[Position, ...]:
        pass

    @abstractmethod
    def is_goal_state(self, state: EatDonutsState) -> bool:
        pass

    @abstractmethod
    def next_states(self, state: EatDonutsState) -> set[State]:
        pass


class CatToDoorProblem(SearchProblem):
    """In this search problem, the cat has to get to the door. The only
    information we need in the state is the position of the cat. We store the
    walls to plan the movement of the cat.
    """

    def __init__(self, game_logic: GameLogic):
        cat_position = game_logic.collect_positions(LabyrinthFields.Cat)[0]
        self.state = State(cat_position)
        self.door_positions = game_logic.collect_positions(LabyrinthFields.Door)
        self.walls = [
            [field == LabyrinthFields.Wall for field in row]
            for row in game_logic.board
        ]
        self.m = len(self.walls)
        self.n = len(self.walls[0])

    def target_positions(self, state: State) -> tuple[Position, ...]:
        return self.door_positions

    def is_goal_state(self, state: State) -> bool:
        return state.position in self.door_positions

    def next_states(self, state: State) -> set[State]:
        ns = set()
        for action in Actions:
            new_position = state.position + Directions[action]
            if (
                0 <= new_position.row < self.m
                and 0 <= new_position.col < self.n
                and not self.walls[new_position.row][new_position.col]
            ):
                ns.add(State(new_position))
        return ns


class EvaluationFunction:
    def __init__(self):
        self.g = dict()

    def __call__(self, node):
        raise NotImplementedError()


# YOUR CODE HERE


def graph_search(problem, f) -> tuple[list[Actions], list[State]]:
    def search_algorithm(problem, f) -> tuple[Optional[State], 
                                              dict[State,Optional[State]],
                                              list[State]]:
        """The graph search algorithm as seen in the lectures."""
        start = problem.start_state()
        G : set[State] = {start}  # we don't need the arcs only the nodes
        g : dict[State,int] = f.g
        g[start] = 0
        OPEN = {start}
        pi : dict[State,Optional[State]] = {start: None} # dictionary to keep track of parent of nodes
        expanded_nodes : list[State] = []
        while True:
            if not OPEN:
                # return the goal node, the dict of parents and expanded nodes
                return None, pi, expanded_nodes
            pass #TODO
            # Idea: Pseudocode from lecture 5 (graphsearch) on slide 10
            #       Make sure to be familiar with sets in Python! (- operator, add)
            #       Add a node to the list of expanded_nodes once a node gets
            #       selected for expanding!
            #       In these search problems, the cost is always 1
            #       (all moves cost the same)

    def reconstruct(goal_node, pi) -> tuple[list[Actions], list[State]]:
        """Reconstructs the path and the actions from the goal node and the
        parents."""
        path = []
        actions = []
        current = goal_node
        while current is not None:
            path.append(current.position)
            current = pi[current]
        current, *rest = path
        while rest:
            prev = current
            current, *rest = rest
            actions.append(RDirections[prev - current])
        return actions, path

    goal_node, pi, expanded_nodes = search_algorithm(problem, f)
    if goal_node:
        actions, path = reconstruct(goal_node, pi)
    else:
        actions = []

    return actions, expanded_nodes


# Search algorithms

# AN EXAMPLE SEARCH
class EvaluationFunctionDFS(EvaluationFunction):
    def __call__(self, node):
        return -self.g[node]

# TODO: Finish these
#       Idea: Definitions from lecture 5 (graphsearch) on slides form 16 to 18
#               Define functions similarly to the __call__ of EvaluationFunctionDFS above!

class EvaluationFunctionBFS(EvaluationFunction):
    def __call__(self, node):
        return node.depth

class EvaluationFunctionLF(EvaluationFunction):
    def __init__(self, h):
        EvaluationFunction.__init__(self)
        self.h = h

    def __call__(self, node):
        return self.h(node)

class EvaluationFunctionAStar(EvaluationFunction):
    def __init__(self, h):
        EvaluationFunction.__init__(self)
        self.h = h

    def __call__(self, node):
        return node.cost + self.h(node)



# The GUI runs the search algorithms through these functions.
# They wouldn't be necessary, but may help understanding.


def depth_first(problem: SearchProblem):
    f = EvaluationFunctionDFS()
    actions, expanded_nodes = graph_search(problem, f)
    return actions, expanded_nodes


def breadth_first(problem: SearchProblem):
    f = EvaluationFunctionBFS()
    actions, expanded_nodes = graph_search(problem, f)
    return actions, expanded_nodes


def look_forward(problem: SearchProblem, h_factory):
    f = EvaluationFunctionLF(h_factory(problem))
    actions, expanded_nodes = graph_search(problem, f)
    return actions, expanded_nodes


def a_star(problem: SearchProblem, h_factory):
    f = EvaluationFunctionAStar(h_factory(problem))
    actions, expanded_nodes = graph_search(problem, f)
    return actions, expanded_nodes


# Heuristics


def straight_line(problem: SearchProblem):
    """Return a function that computes the sum of the straight line distances
    from a position to the target positions
    """

    def heuristics(state: State):
        target_positions = problem.target_positions(state)
        position = state.position
        return sum(
            math.sqrt(
                (position.row - target_position.row) ** 2
                + (position.col - target_position.col) ** 2
            )
            for target_position in target_positions
        )

    return heuristics


def manhattan(problem: SearchProblem):
    """Return a function that computes the sum of the manhattan distances
    from a position to the target positions
    """
    def heuristics(state: State):
        target_positions = problem.target_positions(state)
        position = state.position
        return sum(
            abs(position.row - target_position.row) + abs(position.col - target_position.col)
            for target_position in target_positions
        )
    return heuristics



# Search problem to eat all of the donuts, then get to the exit


class EatDonutsProblem(SearchProblem):
    """In this search problem, the cat has to collect all of the donuts,
    and then get to the door."""

    def __init__(self, game_logic: GameLogic):
        cat_position = game_logic.collect_positions(LabyrinthFields.Cat)[0]
        donuts = game_logic.collect_positions(LabyrinthFields.Donut)
        self.state = EatDonutsState(cat_position, donuts)
        self.door_positions = game_logic.collect_positions(LabyrinthFields.Door)
        self.walls = [
            [field == LabyrinthFields.Wall for field in row]
            for row in game_logic.board
        ]
        self.m = len(self.walls)
        self.n = len(self.walls[0])

    def target_positions(self, state: EatDonutsState) -> tuple[Position, ...]:
        """Return the positions of the donuts if there are any left,
        otherwise return the positions of the doors."""
        if state.donuts:
            return state.donuts
        return self.door_positions

    def is_goal_state(self, state: EatDonutsState) -> bool:
        """The goal is reached when all donuts are eaten and the cat is at the door."""
        return not state.donuts and state.position in self.door_positions

    def next_states(self, state: EatDonutsState) -> set[EatDonutsState]:
        """Return the possible next states based on the cat's movements."""
        ns = set()
        for action in Actions:
            new_position = state.position + Directions[action]
            if (
                    0 <= new_position.row < self.m
                    and 0 <= new_position.col < self.n
                    and not self.walls[new_position.row][new_position.col]
            ):
                # Remove the donut from the list if the new position is a donut
                new_donuts = tuple(
                    donut for donut in state.donuts if donut != new_position
                )
                ns.add(EatDonutsState(new_position, new_donuts))
        return ns


# END OF YOUR CODE

LABYRINTH_DIR = "labyrinths"
labyrinth_fns = sorted(os.listdir(LABYRINTH_DIR))
labyrinth_paths = [
    os.path.join(LABYRINTH_DIR, labyrinth_fn) for labyrinth_fn in labyrinth_fns
]

TILE_DIR = "tiles"
image_paths = {
    LabyrinthFields.Empty: os.path.join(TILE_DIR, "blank_scaled.png"),
    LabyrinthFields.Wall: os.path.join(TILE_DIR, "block_scaled.png"),
    LabyrinthFields.Cat: os.path.join(TILE_DIR, "cat_scaled.png"),
    LabyrinthFields.Door: os.path.join(TILE_DIR, "door_scaled.png"),
    LabyrinthFields.Donut: os.path.join(TILE_DIR, "donut_scaled.png"),
    LabyrinthFields.Bear: os.path.join(TILE_DIR, "bear_scaled.png"),
}
# Use shrinked pngs if the application does not fit on your screen!
# For example: blank_scaled.png --becomes--> blank_scaled_shrinked.png

sg.ChangeLookAndFeel("SystemDefault")

labyrinth_draw_dict = {
    field_type: ("", ("black", "black"), image_paths[field_type])
    for field_type in LabyrinthFields
}

board = LabyrinthBoard(labyrinth_paths[0])
game_logic = GameLogic(board)
problem: SearchProblem = CatToDoorProblem(game_logic)
board_gui = LabyrinthGUI(board, labyrinth_draw_dict)

algorithms: dict[str, Callable] = {
    "Depth first search": depth_first,
    "Breadth first search": breadth_first,
    "Look forward search": look_forward,
    "A* search": a_star,
}

heuristics = {
    "Distance in a straight line": straight_line,
    "Manhattan distance": manhattan,
}

problems: dict[str, Type[SearchProblem]] = {
    "Get to the exit": CatToDoorProblem,
    "Eat all the donuts, then get to the exit": EatDonutsProblem,
}


def create_window(board_gui):
    layout = [
        [sg.Column(board_gui.board_layout)],
        [
            sg.Frame(
                "Algorithm settings",
                [
                    [
                        sg.T("Algorithm: "),
                        sg.Combo(
                            list(algorithms.keys()),
                            key="algorithm",
                            readonly=True,
                            default_value=list(algorithms.keys())[0]
                        ),
                    ],
                    [
                        sg.T("Heuristics: "),
                        sg.Combo(
                            list(heuristics.keys()),
                            key="heuristics",
                            readonly=True,
                            default_value=list(heuristics.keys())[0]
                        ),
                    ],
                ],
            ),
            sg.Frame(
                "Problem settings",
                [
                    [
                        sg.T("Problem:   "),
                        sg.Combo(
                            list(problems.keys()),
                            key="problem",
                            readonly=True,
                            default_value=list(problems.keys())[0]
                        ),
                    ],
                    [
                        sg.T("Labyrinth: "),
                        sg.Combo(
                            labyrinth_paths,
                            key="labyrinth",
                            readonly=True,
                            default_value=labyrinth_paths[0])
                    ],
                ],
            ),
        ],
        [
            sg.T("Steps: "),
            sg.T("0", key="steps", size=(7, 1), justification="right"),
            sg.T("Expanded nodes: "),
            sg.T("0", key="expanded_nodes", size=(7, 1), justification="right"),
        ],
        [sg.Button("Restart"), sg.Button("Step"), sg.Button("Go!"), sg.Button("Exit")],
    ]

    window = sg.Window(
        "Labyrinth",
        layout,
        default_button_element_size=(10, 1),
        auto_size_buttons=False,
        location=(0,0)
    )
    return window


window = create_window(board_gui)

starting = True
go = False
steps = 0

while True:  # Event Loop
    event, values = window.Read(0)
    if event is None or event == "Exit" or event == sg.WIN_CLOSED:
        break
    window.Element("heuristics").Update(
        disabled=values["algorithm"] not in {"Look forward search", "A* search"}
    )
    window.Element("Go!").Update(text="Stop!" if go else "Go!")
    if event == "Restart" or starting:
        if not starting:
            board.load(values["labyrinth"])
            game_logic = GameLogic(board)
            problem = problems[values["problem"]](game_logic)
            board_gui.create()
            window.Close()
            window = create_window(board_gui)
            window.Finalize()
            window.Element("labyrinth").Update(values["labyrinth"])
            window.Element("problem").Update(values["problem"])
            window.Element("algorithm").Update(values["algorithm"])
            window.Element("heuristics").Update(values["heuristics"])
        algorithm = algorithms[values["algorithm"]]
        if algorithm in {look_forward, a_star}:
            heuristic = heuristics[values["heuristics"]]
            algorithm = partial(algorithm, h_factory=heuristic)
        actions, expanded_nodes = algorithm(problem)
        steps = 0
        starting = False
    if event == "Step" or go:
        try:
            action = actions.pop()
            steps += 1
            window.Element("steps").Update(f"{steps}")
            window.Element("expanded_nodes").Update(len(expanded_nodes))
            game_logic.do_action(action)
            board_gui.update()
            board_gui.update_nodes(expanded_nodes)
        except IndexError:
            go = False
    if event == "Go!":
        go = not go

window.Close()
