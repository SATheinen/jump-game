#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "../include/Game.h"
#include "../include/GameState.h"
#include "../include/Config.h"

namespace py = pybind11;

PYBIND11_MODULE(jumpgame, m) {
    m.doc() = "Jump Game RL Environment";

    py::class_<Game>(m, "Game")
        .def(py::init<bool>(), py::arg("headless") = false)
        
        .def("init", &Game::init)
        
        .def("reset", &Game::reset)
        
        .def("clean", &Game::clean)
        
        .def("step", [](Game& self, bool left, bool right, bool jump, int num_frames) {
            GameState gs = self.step(left, right, jump, num_frames);
            
            // Calculate total size: 5 player + NUM_PLATFORMS*4 platform + 1 score + 1 done
            const int total_size = 5 + NUM_PLATFORMS * 4 + 1 + 1;
            auto result = py::array_t<float>(total_size);
            auto buf = result.request();
            float* ptr = static_cast<float*>(buf.ptr);

            // Player state (indices 0-4)
            ptr[0] = gs.player_x;
            ptr[1] = gs.player_y;
            ptr[2] = gs.player_vx;
            ptr[3] = gs.player_vy;
            ptr[4] = gs.player_on_ground ? 1.0f : 0.0f;

            // Platform state (indices 5 onwards)
            int idx = 5;
            for (int i = 0; i < NUM_PLATFORMS; i++) {
                ptr[idx++] = gs.platforms[i][0];  // x
                ptr[idx++] = gs.platforms[i][1];  // y
                ptr[idx++] = gs.platforms[i][2];  // width
                ptr[idx++] = gs.platforms[i][3];  // velocityX
            }
            
            // Score
            ptr[idx] = static_cast<float>(gs.score);

            // done
            ptr[idx] = gs.done ? 1.0f : 0.0f;

            return result;
        },
        py::arg("left") = false,
        py::arg("right") = false, 
        py::arg("jump") = false,
        py::arg("num_frames") = 4);
}