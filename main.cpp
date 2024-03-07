#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "box2d/Box2D.h"

#include "Utils/Utils.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/Physics/WorldHandler.hpp"

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
    tgui::Theme::setDefault("themes/Black.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    WorldHandler::getWorld().SetGravity({0.f,0.f});
    WorldHandler::getWorld().SetContactListener(new CollisionManager);

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
    new Enemy(25,25, player);

    auto healthBar = tgui::ProgressBar::create();
    gui.add(healthBar);
    healthBar->setSize({"25%","5%"});
    healthBar->setPosition({"37.5%",healthBar->getSize().y/2});
    healthBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
    healthBar->setMaximum(player->cast<Player>()->getHealth());
    healthBar->setValue(player->cast<Player>()->getHealth());
    healthBar->setText("Health");

    UpdateManager::Start();
    sf::Clock deltaClock;
    float fixedUpdate = 0;
    sf::Clock timer;
    int frames = 0;
    int fps = 0;
    auto fpsLabel = tgui::Label::create("0");
    gui.add(fpsLabel);
    float secondTimer = 0;
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        secondTimer += deltaTime.asSeconds();
        frames++;
        if (secondTimer >= 1)
        {
            fps = frames;
            frames = 0;
            secondTimer = 0;
            fpsLabel->setText(std::to_string(fps));
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
        UpdateManager::Update(deltaTime.asSeconds());
        if (fixedUpdate >= 0.2)
        {
            fixedUpdate = 0;
            UpdateManager::FixedUpdate();
        }
        UpdateManager::LateUpdate(deltaTime.asSeconds());
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        //! Do physics before this
        WorldHandler::updateWorld(deltaTime.asSeconds());
        CollisionManager::Update();
        //! Draw after this

        //* Write code here

        if (player)
        {
            healthBar->setValue(player->cast<Player>()->getHealth());

            // move the camera toward the player
            b2Vec2 pos = player->getPosition();
            camera.setCenter({camera.getCenter().x - (camera.getCenter().x - pos.x * PIXELS_PER_METER)/20, 
                                camera.getCenter().y - (camera.getCenter().y - pos.y * PIXELS_PER_METER)/20});
            window.setView(camera);

            if (timer.getElapsedTime().asSeconds() >= 2)
            {
                timer.restart();
                new Enemy((rand()%((int)camera.getSize().x) - camera.getSize().x/2)/PIXELS_PER_METER + player->getPosition().x, 
                            (rand()%((int)camera.getSize().y) - camera.getSize().y/2)/PIXELS_PER_METER + player->getPosition().y, player);
            }
        }

        // ---------------

        DrawableManager::draw(window);

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