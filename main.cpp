#include <stdint.h>

#include "psyqo/primitives/common.hh"
#include "third_party/nugget/psyqo/application.hh"
#include "third_party/nugget/psyqo/font.hh"
#include "third_party/nugget/psyqo/gpu.hh"
#include "third_party/nugget/psyqo/scene.hh"
#include "third_party/nugget/psyqo/primitives/rectangles.hh"
#include "psyqo/simplepad.hh"



#define P1_X_POS_INIT 10
#define P1_Y_POS_INIT 115

#define P2_X_POS_INIT 310
#define P2_Y_POS_INIT 115

#define PADDLE_WIDTH 2
#define PADDLE_HEIGHT 15

#define WHITE {{.r = 255, .g = 255, .b = 255}}
#define BLACK {{.r = 0, .g = 0, .b = 0}}


struct Player {

    void getInput(psyqo::SimplePad::Pad pad);
    void move();
    void scorePoint();


    int16_t y_dir;

    short score;

    psyqo::Color color;

    // What gets rendered to the screen
    psyqo::Prim::Rectangle paddle;

    bool is_human_controlled;
};

struct Ball {

    void checkPaddleCollision();
    void checkEdge();
    void move();

    int16_t x_dir;

    int16_t y_dir;

    psyqo::Color color;

    psyqo::Prim::Rectangle shape;

};

// I want a reset state to allow a small pause before play starts
enum PongState {
    PLAY = 0,
    PAUSE = 1,
    RESET = 2
};

class Pong final : public psyqo::Application {

    void prepare() override;
    void createScene() override;

  public:
    psyqo::Font<> m_font;

    psyqo::SimplePad m_pad;
};


class PongScene final : public psyqo::Scene {
    void frame() override;
    void printScores();

    // Execute each state
    void playStateFrame();
    void pauseStateFrame();
    void resetStateFrame();

    public:
    Player p1;
    Player p2;

    Ball ball;

    // init to a reset sequence (kickoff/serve)
    PongState curr_state = PongState::RESET;

    psyqo::Color bg BLACK;
};


Pong pong;
PongScene pongScene;

Ball ball;


void Pong::prepare() {
    psyqo::GPU::Configuration config;
    config.set(psyqo::GPU::Resolution::W320)
        .set(psyqo::GPU::VideoMode::AUTO)
        .set(psyqo::GPU::ColorMode::C15BITS)
        .set(psyqo::GPU::Interlace::PROGRESSIVE);
    gpu().initialize(config);
}

// Called once specifically for the root scene
void Pong::createScene() {
    m_font.uploadSystemFont(gpu());
    
    m_pad.initialize();

    pongScene.p1.paddle.position = {{ .x = P1_X_POS_INIT, .y = P1_Y_POS_INIT }};
    pongScene.p1.paddle.size = {{ .w = PADDLE_WIDTH, .h = PADDLE_HEIGHT }};
    pongScene.p1.color = {{ .r = 0, .g = 255, .b = 0 }};
    pongScene.p1.paddle.setColor(pongScene.p1.color);
    pongScene.p1.is_human_controlled = true;

    pongScene.p2.paddle.position = {{ .x = P2_X_POS_INIT, .y = P2_Y_POS_INIT }};
    pongScene.p2.paddle.size = {{ .w = PADDLE_WIDTH, .h = PADDLE_HEIGHT }};
    pongScene.p2.color = {{ .r = 0, .g = 255, .b = 0 }};
    pongScene.p2.paddle.setColor(pongScene.p2.color);
    pongScene.p1.is_human_controlled = true; // NOTE need to add p2 AI player

    pongScene.ball.shape.position = {{.x = 320 / 2, .y = 240/2}};
    pongScene.ball.shape.size = {{ .w = 2, .h = 2}};
    pongScene.ball.color = {{.r = 255, .g = 255, .b = 255}};
    pongScene.ball.shape.setColor(pongScene.ball.color);

    pushScene(&pongScene);
    // pushScene(&menuScene); // when I create a main menu
}

void PongScene::frame() {
    pong.gpu().clear(this->bg);

    psyqo::Color c = WHITE;    

    // If game is in play...
    // check for collisions, goals scored, update graphics
    
    // if paused...
    // display pause message, disable controls
    
    // if ball is scored...
    // add point to scorer, initiate reset

    pong.gpu().sendPrimitive(ball.shape);
    pong.gpu().sendPrimitive(p1.paddle); 
    pong.gpu().sendPrimitive(p2.paddle);


    switch (curr_state) {
        case PongState::PLAY:
            this->playStateFrame();
            break;
        case PongState::PAUSE:
            this->pauseStateFrame();
            break;
        case PongState::RESET:
            this->resetStateFrame();
            break;
        default:
            break;
    }
}

void PongScene::playStateFrame() {
    // regular gameplay loop

    // Need to experiment as speed would be framerate dependant here
    // TODO make speed frame independant
    ball.x_dir = 1;
    ball.y_dir = 1;

    ball.move();

    ball.checkPaddleCollision();

    p1.getInput(psyqo::SimplePad::Pad1);
}

void PongScene::pauseStateFrame() {
    // stop all movement and only allow unpause input 

    pong.m_font.print(pong.gpu(), "PAUSED - Press 'Circle' to unpause", {{.x = 16, .y = 200}}, WHITE);

    if (pong.m_pad.isButtonPressed(psyqo::SimplePad::Pad1, psyqo::SimplePad::Button::Circle)) {
        this->curr_state = PongState::PLAY;
    }
}

void PongScene::resetStateFrame() {
    // TODO pause for a lil before letting gameplay resume after a goal

    // For now just setting it to play mode until I can implement the delayed start
    this->curr_state = PongState::PLAY;
}

void PongScene::printScores() {
    psyqo::Color score_color = WHITE;
    pong.m_font.print(pong.gpu(), "<SCORE HERE>", {{.x = 16, .y = 32}}, score_color);
}

void Player::getInput(psyqo::SimplePad::Pad pad) {
    if (this->is_human_controlled){ 
        if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Start)) {
            // Pause the game, I.e. set current state to pause

            pongScene.curr_state = PongState::PAUSE;
        }

        if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Up)) {
            this->paddle.position.y -= 1;
        } else if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Down)) {
            this->paddle.position.y += 1;
        }
    }
}

// probably unnecessary
void Player::scorePoint() {
    this->score++;
}

void Ball::checkEdge() {
    // TODO 
}

void Ball::checkPaddleCollision() {
    // TODO 
}

void Ball::move() {
    this->shape.position.x += x_dir;
    this->shape.position.y += y_dir;
}
    
// TODO Create players
// TODO Set P1 and P2 controls
// TODO Draw players
// TODO Create ball
// TODO Draw & move ball
// TODO Set ball collisions with paddles
// TODO Add scoring/reset mechanic
// TODO Draw score
// TODO Add menu screen to select 1 or two players (default to 2 for now)

int main() { return pong.run(); }
