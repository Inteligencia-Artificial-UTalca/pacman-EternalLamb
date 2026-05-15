#include "InkyController.h"
#include <SDL2/SDL.h>

InkyController::InkyController(std::shared_ptr<Character> character):
	Controller(character){
}

InkyController::~InkyController() {
	// TODO Auto-generated destructor stub
}


Move InkyController::getClosestMove(const GameState& game, std::pair<int,int> target)const{
	int minDist=10000000;
	Move minMove=character->getDirection();
	std::vector<Move> moves=game.getMaze().getPossibleMoves(character->getPos());
	for(Move m:moves){
		int vecino = game.getMaze().getNeighbour(character->getPos(),m);
		if(vecino<0)continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int sqDist=euclid2(vecinoCoords,target);
		if(sqDist<minDist){
			minDist=sqDist;
			minMove=m;
		}
	}
	return minMove;
}

Move InkyController::getFarthestMove(const GameState& game, std::pair<int,int> target)const{
	int maxDist=-1;
	Move maxMove=character->getDirection();
	std::vector<Move> moves=game.getMaze().getPossibleMoves(character->getPos());
	for(Move m:moves){
		int vecino = game.getMaze().getNeighbour(character->getPos(),m);
		if(vecino<0)continue;
		auto vecinoCoords = game.getMaze().getNodePos(vecino);
		int sqDist=euclid2(vecinoCoords,target);
		if(sqDist>maxDist){
			maxDist=sqDist;
			maxMove=m;
		}
	}
	return maxMove;
}

float InkyController::getDistanceToPacman(const GameState& game, int g)const{
	return sqrt(euclid2(
		game.getMaze().getNodePos(character->getPos()),
		game.getMaze().getNodePos(game.getGhostsPos(g))));
}

Move
InkyController::getMove(const GameState& game){

	//para cerrar la ventana
	SDL_Event e;
	if( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT || 
			(e.type == SDL_KEYDOWN && 
				(e.key.keysym.sym==SDLK_ESCAPE || 
				e.key.keysym.sym==SDLK_q) ))
		{
			SDL_Quit();
			exit(0);
		}
	}
	
	int InkyNode = character->getPos();
	auto InkyCoords = game.getMaze().getNodePos(InkyNode);

	float fear=0.0f;
	Move escapeMove=PASS;
	float hunger=0.0f;
	Move eatGhostMove=PASS;



	//arrancar de fantasmas cercanos que me pueden comer
			//cambio de formula para que el miedo sea menor
			float tempFear=1.0f-1.0f/(1.0f+pow(2.718f * 0.45f,-getDistanceToPacman(game,1)+50.0f));//logistica
			if(tempFear>fear){
				fear=tempFear;
				escapeMove=getFarthestMove(game,InkyCoords);
			}

	//perseguir fantasmas azules
			//cambio de formula para que el hambre sea mayor
			float tempHunger= (pow(100.0f-getDistanceToPacman(game,1),2)/pow(100.0,2));//cuadratica
			if(tempHunger>hunger){
				hunger=tempHunger;
				eatGhostMove=getClosestMove(game,InkyCoords);;
			}
	//std::cout<<"fear="<<fear<<std::endl;
	//std::cout<<"hunger="<<hunger<<std::endl;
	if(fear>hunger)return escapeMove;
	else return eatGhostMove;
	
}
