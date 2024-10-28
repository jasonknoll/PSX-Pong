#include <stdint.h>

#include "psyqo/primitives/common.hh"
#include "third_party/nugget/common/syscalls/syscalls.h"
#include "third_party/nugget/psyqo/application.hh"
#include "third_party/nugget/psyqo/font.hh"
#include "third_party/nugget/psyqo/gpu.hh"
#include "third_party/nugget/psyqo/scene.hh"
#include "psyqo/simplepad.hh"


struct Player {

    void draw();
    void move();

    int16_t x;
    int16_t y;

    psyqo::Color color;

    bool is_human_controlled;
};

struct Ball {

    void draw();

    int16_t x;
    int16_t y;

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

    Player p1;
    Player p2;

    PongState curr_state;
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

void Pong::createScene() {
    m_font.uploadSystemFont(gpu());
    
    m_pad.initialize();

    pushScene(&pongScene);
}

void PongScene::frame() {
    psyqo::Color c = {{.r = 255, .g = 255, .b = 255}};
    pong.m_font.print(pong.gpu(), "Hello World!", {{.x = 16, .y = 32}}, c);

    // TODO get player input
    if (pong.m_pad.isButtonPressed(psyqo::SimplePad::Pad1, psyqo::SimplePad::Button::Start)) {
        // Pause the game, I.e. set current state to pause
    }

    // If game is in play...

    // if paused...
    
    // if ball is scored...

    // p1.draw();
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
