#include "game.h"
#include <future>

/* Added Main menu feature */
Game::Game(int numWorkerThreads, sf::RenderWindow * window, int numLevels, int enemies, std::string maps) {
	srand(100);
	workerPool = new threadPool(numWorkerThreads);

	isMenuActive = false;
	isPauseActive = false;
	gameActive = false;
	spacebarInactive = false;
	isgameOver = false;
	totalenemies = 10;
	currentLevel = 0;
	totalLevels = numLevels;
	numEnemies = 20;
	mapPaths = maps;
	this->enemies.reserve(40);
	this->gameWindow = window;
	this->gameWindow->setFramerateLimit(30);

	this->enemyspawntimermax = 20;
	this->enemyspawntimer = this->enemyspawntimermax-15;
	
	srikanth = new Hero("images_f/sfr.png@images_f/sfl.png@images_f/sbr.png@images_f/sbl.png@images_f/srr.png@images_f/srl.png@images_f/srm.png@images_f/slr.png@images_f/sll.png@images_f/slm.png@images_f/p1_space.png@", "images_f/laserbeam.png", sf::Vector2f(450.f, 450.f), 100, 10, sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::LShift, this->gameWindow->getSize());
	ujjieve = new Hero("images_f/ufr.png@images_f/ufl.png@images_f/ubr.png@images_f/ubl.png@images_f/urr.png@images_f/url.png@images_f/urm.png@images_f/ulr.png@images_f/ull.png@images_f/ulm.png@images_f/p2_space.png@", "images_f/laserbeam.png", sf::Vector2f(210.f, 320.f), 100, 200, sf::Keyboard::I, sf::Keyboard::K, sf::Keyboard::J, sf::Keyboard::L, sf::Keyboard::RShift, this->gameWindow->getSize());
	this->mainMenu = new Menu(window,0);
	this->pauseMenu = new Menu(window, 1);// To create menu
	this->mainMenuSong = new sf::Music();
	this->storySong = new sf::Music();

	 task1= new std::packaged_task <int()>(std::bind(&Game::loadMenu,this));
	 task2 = new std::packaged_task <int()>(std::bind(&Game::loadSounds, this));
	 task3 = new std::packaged_task <int()>(std::bind(&Game::loadMaps, this));
	 task4 = new std::packaged_task <int()>(std::bind(&Game::loadEnemies, this));
	 f1 =new std::future <int>( task1->get_future());
	f2 = new std::future <int>(task2->get_future());
	f3 = new std::future <int>(task3->get_future());
	f4 = new std::future <int>(task4->get_future());

	std::function<int()> soundJob = std::bind(&Game::loadSounds, this);
	std::function<int()> menuJob = std::bind(&Game::loadMenu, this);
	std::function<int()> mapJob = std::bind(&Game::loadMaps, this);// Once game created it starts laoding all the prerequisites.
	std::function<int()> enemyJob = std::bind(&Game::loadEnemies, this);// Once game created it starts laoding all the prerequisites.

	this->workerPool->addJob(4,soundJob,menuJob,mapJob,enemyJob);
	
	if (!fontStyle.loadFromFile("CHILLER.TTF"));

}
// Method to load the maps 
int Game::loadMaps() {

	int i = 0;
	for (int j = 0; j < totalLevels; j++) {
		
		std::string mapLocation;
		while (mapPaths[i] != '@') mapLocation = mapLocation + mapPaths[i++];
		gameMaps.push_back(Maps(mapLocation, 3840, 3840, 7225));
		i++;
	}

	for (int j = 0; j < totalLevels; j++) {
		gameMaps[j].initialize(j,0.f, 0.f,(float) gameWindow->getSize().x,(float) gameWindow->getSize().y);
	}
	return 0;

}
int Game::loadSounds() {

	if (!gameLoadedBuffer.loadFromFile("sounds_f/gameload.wav") ){
		std::cout << "Error loading the game load sound" << std::endl;
		return EXIT_FAILURE;

	}
	if (!gameOverBuffer.loadFromFile("sounds_f/gameover.wav")) {
		std::cout << "Error loading the game over sound" << std::endl;
		return EXIT_FAILURE;
	}
	gameOverSound.setBuffer(gameOverBuffer);
	gameLoadedSound.setBuffer(gameLoadedBuffer);

	if (!enemyGunSound.loadFromFile("sounds_f/bullet.wav")) {
		std::cout << "Failure to load the bullet sound" << std::endl;

		return EXIT_FAILURE;

	}
	
	if (!storySong->openFromFile("music_f/storymusic.ogg")) {
		std::cout << "Failure to load the music";

		return EXIT_FAILURE;
	}

	return 0;
}
int Game::loadEnemies() {
	/*Method to initialize the enemies*/
	if (!spaceEnemy.loadFromFile("images_f/alienspaceship.png")) {
		std::cout << "Error laoding the enemy tex";
		return EXIT_FAILURE;
	}
	if (!groundEnemy.loadFromFile("images_f/enemy.png")) {
		std::cout << "Error laoding the enemy tex";
		return EXIT_FAILURE;

	}
	if (!bulletEnemy.loadFromFile("images_f/laserbeam.png")) {
		std::cout << "Error loading the bullet texture" << std::endl;
		return EXIT_FAILURE;
	}
	for (int i = 0; i < numEnemies; i++) {
		enemies.push_back(Enemy(rand()% 2,groundEnemy, spaceEnemy,bulletEnemy,enemyGunSound, sf::Vector2f(0, 0), 100, 5, rand() %2,sf::Vector2f(-1.f,0.f), this->gameWindow->getSize()));
		
	}

	for (int i = 0; i < enemies.size(); i++) {
		enemies[i].setLevel(this->currentLevel);
		enemies[i].initialize();
	}
	std::cout << "The number of enemies are " << enemies.size() << std::endl;
	srikanth->setEnemies(this->enemies);
	ujjieve->setEnemies(this->enemies);
	std::cout << "loaded enemies\n";
	return 0;
}

// Method to load the menu
int Game::loadMenu() {
	/* MEthod to load the sprites textures for the players enemies etc..*/
	sf::Texture tex;
	std::cout << "loading menu" << std::endl;
	std::cout << "Loaded files" << std::endl;



	this->mainMenu->loadMenu();
	this->pauseMenu->loadMenu();

	return 0;
}

// Method to display the title screen
int Game::displayTitlescreen() {  /* Function to print the title Screen*/

	if (!mainMenuSong->openFromFile("music_f/mainMenuSong2.ogg")) {
		std::cout << "Failure to load the music";

		return EXIT_FAILURE;
	}

	mainMenuSong->play();
	sf::Texture tex;
	sf::Sprite sp;
	tex.setSmooth(true);
	if (!tex.loadFromFile("images_f/openingimage1.png")) {
		std::cout << "Failure";
		return EXIT_FAILURE;
	}

	
	sp.setTexture(tex);

	this->gameWindow->draw(sp);
	this->gameWindow->display();
	return 0;

}
// Method to display the story
int Game::displayStory() {

	mainMenuSong->pause();
	storySong->play();

	gameWindow->setActive(true);

	std::string image = "images_f/1story.png@images_f/2story.png@images_f/3story.png@images_f/4story.png@";
	std::string waitTime = "6@6@5@4@";

	Image_player img_list(image,waitTime, 4);

	img_list.image_display_function(gameWindow);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	storySong->stop();
	gameWindow->setActive(false);
	return 0;

}
// Method to handle the events generated by key press etc.
void Game::handleEvents() {

	while (gameWindow->pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			gameWindow->close();
	
			break;

		case sf::Event::KeyReleased:

			if (event.key.code == sf::Keyboard::Escape && gameActive) {
				gameActive = false;
				mainMenuSong->play();
				pauseMenu->displayMenu();
				isMenuActive = false;
				isPauseActive = true;
				break;
			}
			else if (event.key.code == sf::Keyboard::E && isMenuActive == false) {
				gameWindow->setActive(true);
				gameActive = true;
				/*Updating the level*/
				this->levelUpdate();
				this->init();
				gameWindow->clear();

			}
			else if (event.key.code == sf::Keyboard::Return && gameActive == false) {
				if (isMenuActive == true) { // Condition for selecting the menu if menu is active
					std::cout << "Menu no." << mainMenu->menuSelected << std::endl;
					std::function<int()> job;
					switch (mainMenu->menuSelected) {
					case 0:
						this->workerPool->waitFinished();
						mainMenuSong->pause();
						gameWindow->setActive(false);
						isMenuActive = false;
						job = std::bind(&Game::displayStory, this);
						workerPool->addJob(1, job);
						break;

					case 1:
						this->workerPool->waitFinished();
						std::cout << "Game started let's play" << std::endl;
						gameActive = true;
						mainMenuSong->pause();
						gameWindow->setActive(true);
						/*Updating the level*/
						this->levelUpdate();
						this->init();
						gameWindow->clear();

						break;
					case 2:
						gameWindow->clear(sf::Color::Black);
						gameWindow->close();
						break;
					}
				}
				else if (isPauseActive) {
					std::cout << "Menu no." << pauseMenu->menuSelected << std::endl;
					std::function<int()> job;
					switch (pauseMenu->menuSelected) {
					case 0:
						gameActive = true;
						//	gameWindow->clear();

						break;

					case 1:

						break;
					case 2:
						gameWindow->clear();
						gameActive = false;
						isPauseActive = false;
						isMenuActive = true;
						this->currentLevel = 0;
						
						mainMenuSong->pause();
						mainMenuSong->play();
						mainMenu->displayMenu();
						break;
					}

				}
				else { // Condition for starting the game after the story is displayd
					std::cout << "Game started let's play" << std::endl;
					gameActive = true;
					gameWindow->setActive(true);
					/*update the level*/

					gameWindow->clear();


				}

			}

			else if (event.key.code == sf::Keyboard::Up && (isMenuActive || isPauseActive)) {
				gameWindow->setActive(true);
				if (isMenuActive)mainMenu->moveUp();
				else pauseMenu->moveUp();
				break;
			}
			else if (event.key.code == sf::Keyboard::Down && (isMenuActive || isPauseActive)) {
				gameWindow->setActive(true);
				if (isMenuActive) mainMenu->moveDown();
				else pauseMenu->moveDown();
				break;
			}

			else if (event.key.code == sf::Keyboard::Space && !spacebarInactive) {
				/*				std::cout << "entered the laoding\n";
								std::cout << "asf " << f1->get() << f2->get() << f3->get() << f4->get();
								std::cout << "loaded\n";
				*/
				printf("Loading ............\n");
				workerPool->waitFinished();
				printf("gameLoaded............\n");
				mainMenuSong->pause();
				mainMenuSong->play();
				isMenuActive = true;
				spacebarInactive = true;
				gameWindow->setActive(false);
				std::function<int()> job = std::bind(&Menu::displayMenu,this->mainMenu);
				//workerPool->addJob(job);
				std::function<int()> hero1Job = std::bind(&Hero::loadFiles, this->srikanth);
				std::function<int()> hero2Job = std::bind(&Hero::loadFiles, this->ujjieve);
				workerPool->addJob(3,job, hero1Job, hero2Job);

				break;
			}

		}
	}
}
int Game::reloadEnemies() {
	for (size_t i = 0; i < enemies.size(); i++) {
		enemies.erase(enemies.begin() + i);
	}
	std::function<int()> enemyJob = std::bind(&Game::loadEnemies, this);// Once game created it starts laoding all the prerequisites.
	workerPool->addJob(1,enemyJob);
	
	return 0;
}
int Game::enemyUpdate() {
	for (size_t i = 0; i < enemies.size(); i++) {
		enemies[i].setLevel(this->currentLevel);
		enemies[i].initialize();
	}
	return 0;
}
int Game::init() {
	std::function<int()> initJob1 = std::bind(&Hero::initialize, this->ujjieve);
	std::function<int()> initJob2 = std::bind(&Hero::initialize, this->srikanth);
	workerPool->addJob(2, initJob1, initJob2);
	ujjieve->setCurrentMap(gameMaps[currentLevel]);
	srikanth->setCurrentMap(gameMaps[currentLevel]);
//	gameMaps[currentLevel].initialize(currentLevel, 0.f, 0.f, (float)gameWindow->getSize().x, (float)gameWindow->getSize().y);

	return 0;
}
int Game::levelUpdate(){
	if (currentLevel<2) currentLevel += 1;
	std::function<int()> enemyJob=std::bind(&Game::enemyUpdate,this);
	workerPool->addJob(1, enemyJob);
	ujjieve->setLevel(this->currentLevel);
	srikanth->setLevel(this->currentLevel);
	return 0;
}
// Method to update the players position health etc.

int Game::playersUpdate() {

//	srikanth->setEnemies(enemies);
	
	srikanth->update();
	ujjieve->update();
	

//	std::function<int()> sriJob = std::bind(&Hero::update, this->srikanth);
//	std::function<int()> ujjJob = std::bind(&Hero::update, this->ujjieve);
//	this->workerPool->addJob(2, sriJob, ujjJob);
	std::cout << "The number of enemies are " << enemies.size() << std::endl;
	this->enemyspawntimer++;
	if (currentLevel==1 && totalenemies<=40 && this->enemyspawntimer >= this->enemyspawntimermax) {
		totalenemies += 1;
		enemies.push_back(Enemy(rand()%2,groundEnemy, spaceEnemy, bulletEnemy,enemyGunSound, sf::Vector2f(0, 0), 100, 5, rand() % 2, sf::Vector2f(-1.f, 0.f), this->gameWindow->getSize()));
		enemies.back().setLevel(this->currentLevel);
		enemies.back().initialize();

		this->enemyspawntimer = 0;

	}
	for (int i = 0; i < enemies.size(); i++) {

		if (enemies[i].playerNr==0) enemies[i].update(srikanth->getPosition());
		else {
			enemies[i].update(ujjieve->getPosition());
		}

		if (ujjieve->getGlobalBounds().intersects(enemies[i].getGlobalBound())) {
			if (currentLevel==1) enemies.erase(enemies.begin() + i);
			enemies[i].takeDamage(40);
			ujjieve->takeDamage(30);
		}
		if (srikanth->getGlobalBounds().intersects(enemies[i].getGlobalBound())) {
			if (currentLevel==1)enemies.erase(enemies.begin() + i);
			enemies[i].takeDamage(40);
			srikanth->takeDamage(30);

		}

		else if (this->enemies[i].getPosition().x < 0 - this->enemies[i].getGlobalBound().width) {
			Enemy oldenemy = enemies[i];
			enemies.erase(enemies.begin() + i);
		//	if (oldenemy.isDead()) enemies.erase(enemies.begin() + i);
		//	if (!oldenemy.isDead()) oldenemy.initialize();
			std::cout << "Erasing the enemies\n";
		

		}

	}

	return 0;
}
// Method to update the map
int Game::mapUpdate() {

	gameMaps[currentLevel].move_background('u', 0.f, ujjieve->getPosition(), srikanth->getPosition());

	return 0;

}
// Method to render the map
void Game::mapRender() {
	gameMaps[currentLevel].renderMap(this->gameWindow);

}
// Method to render the players
void Game::playerRender() {
	srikanth->draw(this->gameWindow);
	ujjieve->draw(this->gameWindow);
	for (size_t i = 0; i < enemies.size(); i++) {
		enemies[i].draw(this->gameWindow);
	}
}

// Method for updation 
void Game::update() {
	//Both players should be alive else game over
	if (srikanth->isAlive && ujjieve->isAlive) {
		std::function<int()> mapJob = std::bind(&Game::mapUpdate, this);
		workerPool->addJob(1, mapJob);
		//	playersUpdate();
		std::function<int()> playerJob = std::bind(&Game::playersUpdate, this);
		workerPool->addJob(1, playerJob);
	}
	else {// Game Over  
		runGame = false;
		gameActive = false;
//		gameWindow->setActive(false);

		std::function<int()> gameOverJob = std::bind(&Game::displayGameOver, this);
		workerPool->addJob(1, gameOverJob);
	//	isMenuActive = true;
	}
}

// Method for rendering purposes
void Game::render() {
	
		if (gameActive || isgameOver)	this->workerPool->waitFinished();
//		gameWindow->setActive(true);
		gameWindow->clear();
		if (!isgameOver) {
			mapRender();
			playerRender();
		}
		gameWindow->display();
		

}
int Game::restart() {
	std::function<int()> enemyJob = std::bind(&Game::reloadEnemies, this);// Once game created it starts laoding all the prerequisites.
	workerPool->addJob(1, enemyJob);
	isMenuActive = true;
	runGame = true;
	isgameOver = false;
	mainMenuSong->play();
	mainMenu->displayMenu();

	this->currentLevel = 0;
	srikanth->isAlive = true;
	ujjieve->isAlive = true;
	return 0;
}
int Game::displayGameOver() {
//	gameWindow->setActive(true);
	gameOverSound.play();
	gameOverText.setString("GAME OVER PLEASE TRY AGAIN");
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setCharacterSize(300);
	gameWindow->draw(gameOverText);
	isgameOver = true;
//	gameWindow->display();
//	restart();
	return 0;
}
Game::~Game() {
	this->workerPool->waitFinished();
	this->workerPool->destroy();
}

