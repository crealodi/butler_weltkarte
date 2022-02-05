#include "PuzzleStateMachine.h"

StateFunc puzzleState = (StateFunc)puzzle_init;

void setup() { }

void loop()
{
	puzzleState = (StateFunc)(*puzzleState)();
}

