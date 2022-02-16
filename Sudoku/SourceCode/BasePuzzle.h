#pragma once
#include<iostream>
#include"cocos2d.h"
#include"BaseCube.h"
//#include"Global.h"
USING_NS_CC;

class BasePuzzle {
public:
	BasePuzzle();
	~BasePuzzle();

	//get Puzzle and Answer with its vector's position


public:
	BaseCube* cube_undo_1;
	BaseCube* cube_selected_1;
	BaseCube* cube_filled_1;

	BaseCube* cube_undo_2;
	BaseCube* cube_selected_2;
	BaseCube* cube_filled_2;

	BaseCube* cube_undo_3;
	BaseCube* cube_selected_3;
	BaseCube* cube_filled_3;

	BaseCube* cube_undo_4;
	BaseCube* cube_selected_4;
	BaseCube* cube_filled_4;

	BaseCube* cube_seleced_0;
	BaseCube* cube_blank_0;

	BaseCube* arr_puz[4][4];
	BaseCube* arr_ans[4][4];
};
