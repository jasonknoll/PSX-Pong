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


struct Player {

    void getInput(psyqo::SimplePad::Pad pad);
    void move();

    int16_t x;
    int16_t y;
    int16_t y_dir;

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

    public:
    Player p1;
    Player p2;

    PongState curr_state;

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

    // TODO set player init positions
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

    pushScene(&pongScene);
}

void PongScene::frame() {
    pong.gpu().clear(this->bg);

    psyqo::Color c = {{.r = 255, .g = 255, .b = 255}};
    pong.m_font.print(pong.gpu(), "Hello World!", {{.x = 16, .y = 32}}, c);

    // TODO get player input
    p1.getInput(psyqo::SimplePad::Pad1);

    // If game is in play...

    // if paused...
    
    // if ball is scored...

    pong.gpu().sendPrimitive(p1.paddle);
    pong.gpu().sendPrimitive(p2.paddle);
}

void Player::getInput(psyqo::SimplePad::Pad pad) {
    if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Start)) {
        // Pause the game, I.e. set current state to pause
    }

    if (pong.m_pad.isButtonPressed(pad, psyqo::SimplePad::Button::Up)) {
        this->paddle.position.y -= 1;
    }
}

// TODO Create players
// TODO Set P1 and P2 controls
// TODO Draw players
// TODO Create ball
// TODO Draw & move ball
// TODO Set ball collisions with paddles
// TODO Add scoring/reset mechanic
// TODO Draw score

int main() { return pong.run(); }
