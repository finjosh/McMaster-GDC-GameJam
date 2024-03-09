#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "box2d/Box2D.h"

#include "Utils/Utils.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/iniParser.hpp"

#include "Utils/UpdateManager.hpp"
#include "Utils/Graphics/DrawableManager.hpp"
#include "Utils/ObjectManager.hpp"
#include "Utils/Physics/CollisionManager.hpp"

#include "Player.hpp"
#include "Enemy.hpp"

using namespace std;
using namespace sf;

void addThemeCommands();

class Wall : public virtual Object, public Collider, public DrawableObject, public sf::RectangleShape
{
public:
    inline Wall(const b2Vec2& pos, const b2Vec2& size)
    {
        b2PolygonShape b2shape;
        b2shape.SetAsBox(size.x/2, size.y/2);

        Collider::initCollider(pos.x,pos.y);
        Collider::createFixture(b2shape, 1, 0.25);
        Collider::getBody()->SetType(b2BodyType::b2_staticBody);

        RectangleShape::setSize({size.x*PIXELS_PER_METER,size.y*PIXELS_PER_METER});
        RectangleShape::setOrigin(size.x/2*PIXELS_PER_METER,size.y/2*PIXELS_PER_METER);
        RectangleShape::setFillColor(sf::Color::Red);
    }

    inline virtual void Draw(sf::RenderWindow& window) override
    {
        RectangleShape::setPosition(Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER);
        RectangleShape::setRotation(Object::getRotation()*180/PI);
        window.draw(*this);
    }

    createDestroy();
};

void loadMainMenu(tgui::Gui& gui, tgui::ProgressBar::Ptr& healthBar, Object::Ptr<>& player, bool& _playingGame, bool& _controlsPage, const std::string& bestTime, const std::string& lastTime, tgui::ProgressBar::Ptr& waveBar);

// TODO setup a view manager that handles windows size changes
int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Example Game", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    WindowHandler::setRenderWindow(&window);
    sf::View camera(sf::FloatRect{0,0,1920,1080});
    window.setView(camera); // TODO make camera its own class

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("Assets/themes/Black.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    WorldHandler::getWorld().SetGravity({0.f,0.f});
    WorldHandler::getWorld().SetContactListener(new CollisionManager); // TODO put this stuff into the constructor

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);

    //! ---------------------------------------------------

    Object::Ptr<> player = new Player(10,10,10);
    camera.setCenter(player->getPosition().x*PIXELS_PER_METER, player->getPosition().y*PIXELS_PER_METER);
    sf::RectangleShape _inGameBackground;
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("Assets/background.png");
    _inGameBackground.setTexture(&backgroundTexture);
    _inGameBackground.setOrigin({(float)backgroundTexture.getSize().x/2, (float)backgroundTexture.getSize().y/2});
    _inGameBackground.setSize({(float)backgroundTexture.getSize().x, (float)backgroundTexture.getSize().y});
    b2Vec2 _gameBackgroundPos;

    std::list<Object::Ptr<>> _enemies;

    tgui::ProgressBar::Ptr healthBar;
    tgui::ProgressBar::Ptr nextWave;

    bool _playingGame = false;
    bool _controlsPage = false;

    iniParser ini;
    ini.setFilePath("Assets/BestTime.txt");
    if (!ini.isOpen())
    {
        ini.createFile("Assets/BestTime.txt");
        ini.setFilePath("Assets/BestTime.txt");        
    }
    if (!ini.LoadData())
    {
        ini.overrideData();
        ini.addSection("General");
        ini.addValue("General", "Best Time", "NA");
        ini.addValue("General", "Last Time", "NA");
    }
    std::string BestTime = ini.getValue("General", "Best Time");
    std::string LastTime = ini.getValue("General", "Last Time");

    loadMainMenu(gui, healthBar, player, _playingGame, _controlsPage, BestTime, LastTime, nextWave);

    UpdateManager::Start();
    sf::Clock deltaClock;
    float fixedUpdate = 0;
    float timer = 0;
    int spawnAmount = 1;
    float spawnTimer = 1;
    float secondTimer = 0;
    float lifeTimeTimer;
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        secondTimer += deltaTime.asSeconds();
        if (secondTimer >= 1)
        {
            secondTimer = 0;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();

            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            Command::Prompt::UpdateEvent(event);
            //! ----------------------------------------------------------
        }
        if (_playingGame || _controlsPage)
        {
            UpdateManager::Update(deltaTime.asSeconds());
            if (fixedUpdate >= 0.2)
            {
                fixedUpdate = 0;
                UpdateManager::FixedUpdate();
            }
            UpdateManager::LateUpdate(deltaTime.asSeconds());
        }
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        //! Do physics before this
        if (_playingGame || _controlsPage)
        {
            WorldHandler::updateWorld(deltaTime.asSeconds());
            CollisionManager::Update();
        }
        //! Draw after this

        //* Write code here

        if (_controlsPage)
        {
            camera.setCenter({camera.getSize().x/2,camera.getSize().y/2});
            window.setView(camera);
        }
        if (player && _playingGame)
        {
            lifeTimeTimer += deltaTime.asSeconds();
            timer += deltaTime.asSeconds();
            if (player->cast<Player>()->getHealth() == 0)
            {
                BestTime = lifeTimeTimer > StringHelper::toFloat(BestTime) ? StringHelper::FloatToStringRound(lifeTimeTimer, 2) + "s" : BestTime;
                LastTime = StringHelper::FloatToStringRound(lifeTimeTimer) + "s";
                ini.setValue("General", "Best Time", BestTime);
                ini.setValue("General", "Last Time", LastTime);
                lifeTimeTimer = 0;

                _playingGame = false;
                auto gameOverLabel = tgui::Label::create("Game Over!");
                gui.add(gameOverLabel);
                gameOverLabel->setTextSize(75);
                gameOverLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                gameOverLabel->setSize({"100%", gameOverLabel->getSize().y});
                gameOverLabel->setPosition({"0", "30%"});
                auto mainMenu = tgui::Button::create("Main Menu");
                gui.add(mainMenu);
                mainMenu->setTextSize(75);
                mainMenu->setSize({"25%","15%"});
                mainMenu->setPosition({"37.5%", "55%"});
                mainMenu->onClick([&gui, &healthBar, &player, &_playingGame, &_enemies, &_controlsPage, &BestTime, &LastTime, &nextWave]()
                {
                    gui.removeAllWidgets();
                    player->cast<Player>()->setHealth(10);
                    for (auto enemy: _enemies)
                    {
                        if (enemy)
                            enemy->destroy();
                    }
                    _enemies.clear();
                    loadMainMenu(gui, healthBar, player, _playingGame, _controlsPage, BestTime, LastTime, nextWave);
                });
            }
            healthBar->setValue(player->cast<Player>()->getHealth());
            nextWave->setValue(timer/spawnTimer*1000);

            // move the camera toward the player
            b2Vec2 pos = player->getPosition();
            camera.setCenter({camera.getCenter().x - (camera.getCenter().x - pos.x * PIXELS_PER_METER)/20, 
                                camera.getCenter().y - (camera.getCenter().y - pos.y * PIXELS_PER_METER)/20});
            window.setView(camera);

            if (timer >= spawnTimer)
            {
                spawnTimer += spawnAmount/spawnTimer*8;
                nextWave->setValue(0);
                spawnAmount++;
                timer = 0;
                for (int i = 0; i < spawnAmount; i++)
                {
                    _enemies.emplace_back(new Enemy((rand()%((int)camera.getSize().x) - camera.getSize().x/2)/PIXELS_PER_METER + player->getPosition().x, 
                                                    (rand()%((int)camera.getSize().y) - camera.getSize().y/2)/PIXELS_PER_METER + player->getPosition().y, player));
                }
            }
        }

        // ---------------

        if (_playingGame || _controlsPage) 
        {
            if (_gameBackgroundPos.x + (float)backgroundTexture.getSize().x/2 < player->getPosition().x*PIXELS_PER_METER)
            {
                _gameBackgroundPos.x += (float)backgroundTexture.getSize().x;
            }
            else if (_gameBackgroundPos.x - (float)backgroundTexture.getSize().x/2 > player->getPosition().x*PIXELS_PER_METER)
            {
                _gameBackgroundPos.x -= (float)backgroundTexture.getSize().x;
            }
            if (_gameBackgroundPos.y + (float)backgroundTexture.getSize().y/2 < player->getPosition().y*PIXELS_PER_METER)
            {
                _gameBackgroundPos.y += (float)backgroundTexture.getSize().y;
            }
            else if (_gameBackgroundPos.y - (float)backgroundTexture.getSize().y/2 > player->getPosition().y*PIXELS_PER_METER)
            {
                _gameBackgroundPos.y -= (float)backgroundTexture.getSize().y;
            }

            for (int i = -4; i < 5; i++)
            {
                for (int j = -3; j < 4; j++)
                {
                    _inGameBackground.setPosition({_gameBackgroundPos.x + i*(int)_inGameBackground.getTexture()->getSize().x, _gameBackgroundPos.y + j*(int)_inGameBackground.getTexture()->getSize().y});
                    window.draw(_inGameBackground);
                }
            }

            DrawableManager::draw(window);
        }

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    Command::Prompt::close();
    TFuncDisplay::close();
    //! --------------------------------------------------------------

    window.close();

    return 0;
}

void addThemeCommands()
{
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"},
        std::list<Command::command>{
            Command::command{"default", "(Currently does not work, coming soon) Sets the theme back to default", {[](){ 
                tgui::Theme::setDefault(""); //! This does not work due to a tgui issue
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}},
            // Dark theme is a custom theme made by me 
            // It can be found here: https://github.com/finjosh/TGUI-DarkTheme
            Command::command{"dark", "Sets the them to the dark theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Dark.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}}, 
            Command::command{"black", "Sets the them to the black theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Black.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}},
            Command::command{"grey", "Sets the them to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/TransparentGrey.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}}
        }
    });
}

void loadMainMenu(tgui::Gui &gui, tgui::ProgressBar::Ptr &healthBar, Object::Ptr<> &player, bool &_playingGame, bool& _controlsPage, const std::string& bestTime, const std::string& lastTime, tgui::ProgressBar::Ptr& waveBar)
{
    gui.loadWidgetsFromFile("Assets/MainMenu.txt");
    gui.get("StartButton")->cast<tgui::Button>()->onClick([&gui, &healthBar, &player, &_playingGame, &waveBar]()
    { 
        gui.removeAllWidgets(); 
        healthBar = tgui::ProgressBar::create();
        gui.add(healthBar);
        healthBar->setSize({"25%","5%"});
        healthBar->setPosition({"37.5%",healthBar->getSize().y/2});
        healthBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
        healthBar->setMaximum(player->cast<Player>()->getHealth());
        healthBar->setValue(player->cast<Player>()->getHealth());
        healthBar->setText("Health");
        waveBar = tgui::ProgressBar::create();
        gui.add(waveBar);
        waveBar->setSize({"40%","2.5%"});
        waveBar->setPosition({"30%","97.5%"});
        waveBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
        waveBar->setMaximum(1000);
        waveBar->setValue(0);
        waveBar->setText("Next Wave");
        _playingGame = true;
        player->cast<Player>()->removeBullets();
    });
    gui.get("ControlsButton")->cast<tgui::Button>()->onClick([&gui, &_controlsPage, &healthBar, &player, &_playingGame, bestTime, lastTime, &waveBar]()
    {
        _controlsPage = true;
        gui.removeAllWidgets();
        gui.loadWidgetsFromFile("Assets/Controls.txt");
        player->setPosition({1920/2/PIXELS_PER_METER,1080/2/PIXELS_PER_METER});
        size_t walls[4];
        walls[0] = (new Wall({0,-10},{1000,20}))->getID();
        walls[1] = (new Wall({0,(float)WindowHandler::getRenderWindow()->getView().getSize().y/PIXELS_PER_METER + 10},{1000,20}))->getID();
        walls[2] = (new Wall({-10,0},{20,1000}))->getID();
        walls[3] = (new Wall({(float)WindowHandler::getRenderWindow()->getView().getSize().x/PIXELS_PER_METER + 10,0},{20,1000}))->getID();
        player->cast<Player>()->removeBullets();
        gui.get("MainMenuButton")->cast<tgui::Button>()->onClick([&gui, &_controlsPage, &healthBar, &player, &_playingGame, walls, bestTime, lastTime, &waveBar]()
        {
            _controlsPage = false;
            gui.removeAllWidgets();
            loadMainMenu(gui, healthBar, player, _playingGame, _controlsPage, bestTime, lastTime, waveBar);
            for (int i = 0; i < 4; i++)
            {
                ObjectManager::getObject(walls[i])->destroy();
            }
            player->cast<Player>()->removeBullets();
        });
    });
    gui.get("Best Time")->cast<tgui::Label>()->setText("Best Time: " + bestTime);
    gui.get("Last Time")->cast<tgui::Label>()->setText("Last Time: " + lastTime);
}
