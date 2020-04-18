// Copyright (C) 2019 Yueh-Ting Chen (eopXD)
/*! \file greedy.cpp
	\brief baseline agents
	 -D GREEDY, greedy movement evaluated by eval() below
	 -D CONSERVATIVE, conservative strategy, eats opponent, or do hor/vert,
	  if none of the above move exists, do random
	 -D RANDOM, do random move from move_list()
	 Add log: -D PLAYER1/PLAYER2
	\author Yueh-Ting Chen (eopXD)
	\course Theory of Computer Game (TCG)
*/

#include <cstdlib>
#include <ctime>
#include <cmath>

#include <iostream>
#include <fstream>
#include <utility>
#include <chrono>


#include "einstein.hpp"

char start;
char init[2][NUM_CUBE+1] = {};
BOARD_GUI *b, tmp_b;
bool myturn;
inline void flip_bit ( bool &_ ) { _ = !_; }
char num, dir;
std::fstream flog;
void logger ( std::string logfile ) {
	flog.open(logfile, std::fstream::out);
	if ( !flog.is_open() ) {
		throw std::runtime_error("error opening file\n");
	}
}
using PII = std::pair<int, int>;

#ifdef GREEDY
#define EAT 5.0 // threatened 
#define ATTACK 1.0 // my minimum piece distance to goal
#define DEFENSE 1.0 // opponent's piece to goad

#define DRAW 50
#define WIN 100

double eval ( BOARD_GUI *eval_b ) { // evaluating a board
	if ( eval_b->winner()!=Color::OTHER ) {
		if ( eval_b->winner() == eval_b->turn() ) { // lose
			return (-WIN);
		}
		else if ( eval_b->winner() == Color::NO_ONE ) { // draw
			return (DRAW);
		}
		else {
			return (WIN);
		}
	}

	Color opponent = eval_b->turn();
	
	double threatened = 0.0;
	double distance[2] = {0.0, 0.0};
	
	auto ml = eval_b->move_list();
	for ( auto &move: ml ) {
		if ( eval_b->yummy(move) == 1 ) {
			++threatened;
		}
	}
	for ( int ply=0; ply<2; ++ply ) {
	for ( int num=0; num<NUM_CUBE; ++num ) {
		PII pos = eval_b->find(static_cast<Color>(ply), num);
		if ( pos.first == -100 ) { continue; }
		PII corner = (ply==0)?std::make_pair(BOARD_SZ-1, BOARD_SZ-1):std::make_pair(0, 0);
		double coefficient = (ply==enum2int(opponent))?DEFENSE:ATTACK;
		distance[ply] += pow(NUM_CUBE-num, coefficient) * (abs(pos.first-corner.first)+abs(pos.second-corner.second));
	}}
	double res = 0;
	res -= threatened*EAT;
	res -= DEFENSE*distance[enum2int(opponent)];
	res += ATTACK*distance[!enum2int(opponent)];
	return (res);
}
#endif

int main () 
{
#ifdef GREEDY
	logger(".log.greedy");
#endif
#ifdef CONSERVATIVE
	logger(".log.conservative");
#endif
#ifdef RANDOM
	logger(".log.random");
#endif

	auto timer = [] ( bool reset = false ) -> double {
		static decltype(std::chrono::steady_clock::now()) tick, tock;
		if ( reset ) {
			tick = std::chrono::steady_clock::now();
			return (0);
		}
		tock = std::chrono::steady_clock::now();
		return (std::chrono::duration_cast<std::chrono::duration<double>>(tock-tick).count());
	};

	srand(time(NULL));

	do {
		/* get initial positions */
		for ( int i=0; i<2; ++i ) { for ( int j=0; j<NUM_CUBE; ++j ) {
				init[i][j] = getchar();
		}}
		init[0][NUM_CUBE] = init[1][NUM_CUBE] = '\0';
		start = getchar();

		flog << init[0] << " " << init[1] << std::endl;
		flog << start << std::endl;

		/* game start s*/
		b = new BOARD_GUI(init[0], init[1]);
		b->no_hl = 1;

		for ( myturn=(start=='f'); b->winner()==Color::OTHER; flip_bit(myturn) ) {
			if ( myturn ) {
#ifdef GREEDY
				flog << "origin\n";
				flog << *(b);
				auto ml = b->move_list();
				PII m;
				double val = -10000;
				for ( auto &move: ml ) {
					b->do_move(move);
					double tmp = eval(b);
					flog << *(b);
					flog << "val: " << tmp << std::endl;
					if ( tmp > val ) {
						m = move;
						val = tmp;
					}
					b->undo_move();
				}
#endif
#ifdef CONSERVATIVE
				auto ml = b->move_list();
				PII m = PII(-1, -1);
				for ( auto &move: ml ) {
					if ( b->yummy(move) == 1 ) {
						m = move;
						break;
					}
				}
				if ( m.first == -1 ) {
					for ( auto &move: ml ) {
						if ( move.second<2 and b->yummy(move) != -1 ) {
							m = move;
							break;
						}
					}
				}
				if ( m.first == -1 ) {
					m = ml[rand()%ml.size()];
				}
#endif
#ifdef RANDOM
				auto ml = b->move_list();
				auto m = ml.at(rand()%ml.size());				
#endif		
				flog << myturn << b->send_move(m) << std::endl;
				b->do_move(m);
				std::cout << b->send_move(m) << std::flush;
			}
			else {
				num = getchar()-'0';
				dir = getchar()-'0';
				flog << myturn << (int)num << (int)dir << std::endl;
				if ( num == 16 ) {
					b->undo_move();
					b->undo_move();
					flip_bit(myturn);
				}
				else {
					b->do_move(num, dir);
				}
			}
		}
		flog << "winner: " << b->winner() << std::endl;
		/* game end */
		delete b;
	} while ( getchar()=='y' ); 

	return (0);
}