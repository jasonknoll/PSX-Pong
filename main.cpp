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


struct Player {

    void getInput(psyqo::SimplePad::Pad pad);
    void move();
    void scorePoint();

    int16_t x;
    int16_t y;
    int16_t y_dir;

    short score;

    psyqo::Color color;

    // What gets rendered to the screen
    psyqo::Prim::Rectangle paddle;

    bool is_human_controlled;
};

struct Ball {

    void move();

    int16_t x;
    int16_t x_dir;

    int16_t y;
    int16_t y_dir;

    psyqo::Color color;

    psyqo::Prim::Rectangle shape;

};

// I want a reset state to allow a small pause before play starts
enum PongState {
    PLAY,
    PAUSE,
    RESET
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

    public:
    Player p1;
    Player p2;

    Ball ball;

    // init to a reset sequence (kickoff/serve)
    PongState curr_state = PongState::RESET;

    psyqo::Color bg {{.r = 0, .g = 0, .b = 0}};
};


Pong pong;
PongScene pongScene;


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
}

void PongScene::frame() {
    pong.gpu().clear(this->bg);

    psyqo::Color c = {{.r = 255, .g = 255, .b = 255}};
    pong.m_font.print(pong.gpu(), "Hello World!", {{.x = 16, .y = 32}}, c);

    // TODO get player input
    p1.getInput(psyqo::SimplePad::Pad1);

    // If game is in play...
    // check for collisions, goals scored

    // if paused...
    // display pause message, disable controls
    
    // if ball is scored...
    // add point to scorer, initiate reset

    pong.gpu().sendPrimitive(ball.shape);
    pong.gpu().sendPrimitive(p1.paddle);
    pong.gpu().sendPrimitive(p2.paddle);
}

void PongScene::printScores() {
    psyqo::Color score_color = WHITE;
    pong.m_font.print(pong.gpu(), "<SCORE HERE>", {{.x = 16, .y = 32}}, score_color);
}

void Player::getInput(psyqo::SimplePad::Pad pad) {
    if (this->is_human_controlled){ 
        if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Start)) {
            // Pause the game, I.e. set current state to pause
        }

        if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Up)) {
            this->paddle.position.y -= 1;
        } else if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Down)) {
            this->paddle.position.y += 1;
        }
    }
}

void Player::scorePoint() {
    this->score++;
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
