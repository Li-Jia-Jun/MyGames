#include"BasePuzzle.h"

BasePuzzle::BasePuzzle()
{
	//initialize every cube needed

	//create temp arrs and push back to vector
	BaseCube*puz[4][4] = { { new BaseCube(0, TYPE_BLANK, "BaseCube/cube_blank_0.png"),new BaseCube(3,TYPE_UNDO,"BaseCube/cube_undo_3"),new BaseCube(1,TYPE_UNDO,"Game/filled_1"),new BaseCube(0,TYPE_BLANK,"Game/cube_blank_0") },
													 { new BaseCube(1,TYPE_UNDO,"BaseCube/cube_undo_1"),new BaseCube(0, TYPE_BLANK, "BaseCube/cube_blank_0.png"),new BaseCube(0, TYPE_BLANK, "BaseCube/cube_blank_0.png"),new BaseCube(3,TYPE_UNDO,"Game/cube_undo_3") },
													{ new BaseCube(2,TYPE_UNDO,"BaseCube/cube_undo_2"),new BaseCube(0, TYPE_BLANK, "BaseCube/cube_blank_0.png"),new BaseCube(3,TYPE_UNDO,"BaseCube/cube_undo_3"),new BaseCube(4,TYPE_UNDO,"Game/cube_undo_4") },
													{ new BaseCube(0, TYPE_BLANK, "BaseCube/cube_blank_0.png"),new BaseCube(4,TYPE_UNDO,"BaseCube/cube_undo_4"),new BaseCube(2,TYPE_UNDO,"BaseCube/cube_undo_2"),new BaseCube(0, TYPE_BLANK, "BaseCube/cube_blank_0.png") } };

	BaseCube* ans[4][4] = { { new BaseCube(4,TYPE_FILLED,"BaseCube/cube_filled_4"),new BaseCube(3,TYPE_UNDO,"BaseCube/cube_undo_3"),new BaseCube(1,TYPE_UNDO,"BaseCube/cube_undo_1"),new BaseCube(2,TYPE_FILLED,"BaseCube/cube_filled_2") },
												   { new BaseCube(1,TYPE_UNDO,"BaseCube/cube_undo_1"),new BaseCube(2,TYPE_FILLED,"BaseCube/cube_filled_2"),new BaseCube(4,TYPE_FILLED,"BaseCube/cube_filled_4"),new BaseCube(3,TYPE_UNDO,"BaseCube/cube_undo_3") },
												   { new BaseCube(2,TYPE_UNDO,"BaseCube/cube_undo_2"), new BaseCube(1,TYPE_FILLED,"BaseCube/cube_filled_1"),new BaseCube(3,TYPE_UNDO,"BaseCube/cube_undo_3"),new BaseCube(4,TYPE_UNDO,"BaseCube/cube_undo_4") },
												   { new BaseCube(3,TYPE_FILLED,"BaseCube/cube_filled_3"),new BaseCube(4,TYPE_UNDO,"BaseCube/cube_undo_4"),new BaseCube(2,TYPE_UNDO,"BaseCube/cube_undo_2"), new BaseCube(1,TYPE_FILLED,"BaseCube/cube_filled_1") } };

	//add puzzle&ans to Global  

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
		arr_puz[i][j] = puz[i][j];
		arr_ans[i][j] = ans[i][j];
		}
	}
};

BasePuzzle::~BasePuzzle()
{

}

