"""Base classes for the boards of the exercises"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class Position:
    row: int
    col: int

    def __add__(self, other: Position) -> Position:
        return Position(self.row + other.row, self.col + other.col)

    def __sub__(self, other: Position) -> Position:
        return Position(self.row - other.row, self.col - other.col)


class Board:
    """A base class for an m by n board. It stores the board as a list of lists. The
    board can be indexed into as board[i][j] or board[i, j], or with a Position
    object.

    """

    def __init__(self, m: int, n: int):
        self.m = m
        self.n = n
        self.create_board()

    def create_board(self) -> None:
        self.board = []
        for i in range(self.m):
            row = []
            for j in range(self.n):
                row.append(self._default_state_for_coordinates(i, j))
            self.board.append(row)

    def __iter__(self):
        return iter(self.board)

    def __getitem__(self, index: tuple[int, int] | Position | int) -> Any:
        if isinstance(index, tuple):
            i, j = index
            return self.board[i][j]
        elif isinstance(index, Position):
            return self.board[index.row][index.col]
        else:
            return self.board[index]

    def __setitem__(self, index: tuple[int, int] | Position | int, item) -> None:
        if isinstance(index, tuple):
            i, j = index
            self.board[i][j] = item
        elif isinstance(index, Position):
            self.board[index.row][index.col] = item
        else:
            self.board[index] = item

    def _default_state_for_coordinates(self, i: int, j: int):
        raise NotImplementedError()
