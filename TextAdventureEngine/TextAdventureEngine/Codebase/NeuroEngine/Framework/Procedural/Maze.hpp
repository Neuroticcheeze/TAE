#pragma once
#ifndef MAZE_H
#define	MAZE_H

//=====================================================================================
//!	\brief Represents a uniform grid of walls and empty spaces, as well as more advanced
//!		   features such as identified doorways, weak walls, etc.
class Maze
{
public:

	Maze();
	~Maze();



private:

	// Dimensions of the maze
	uint32_t m_Width = 0, m_Height = 0;

	uint8_t * m_MazeData = nullptr;
};

#endif//MAZE_H