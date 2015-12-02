/**
 * Author: Hongwei Gao
 * This is a console game simulating the board game - Mastermind.
 * In this game it uses the number to represent the color of the code peg.
 * So if there are 8 colors, the numbers from 0 to 7 are used to represent corresponding color.
 * To build it -- "g++ -std=c++11 -o mastermind mastermind.cpp"
 */
#include <iostream>
#include <random>
#include <array>
#include <string.h>

using namespace std;

namespace {
    constexpr uint32_t PEG_SLOTS = 4;
    constexpr uint32_t MIN_PEG_COLORS = 2;
    constexpr uint32_t MAX_PEG_COLORS = MIN_PEG_COLORS + 8;
    constexpr uint32_t MIN_TRIES_ALLOWED = 2;
}


class Mastermind {
public:
    /**
     * @brief Mastermind
     * @param colors -- [2 -- 10] default is 8
     * @param tries  -- [2 -- >> default is 10
     */
    explicit Mastermind(uint32_t colors = 8, uint32_t tries = 10)
        : m_peg_colors{colors}, m_tries_allowed{tries} {
        // Generate the hidden pegs
        static uniform_int_distribution<uint32_t> u(0, m_peg_colors - 1);
        static default_random_engine e(time(0));
        for (uint32_t i = 0; i < PEG_SLOTS; ++i) {
            m_hidden_pegs[i] = u(e);
        }
    }

    // Not need them
    Mastermind(const Mastermind &) = delete;
    Mastermind & operator=(const Mastermind &) = delete;
    Mastermind(Mastermind &&) = delete;
    Mastermind & operator=(Mastermind &&) = delete;

    /**
     * @brief guess the hidden pegs
     * @return true -- the guess is right, false -- the guess is wrong
     */
    bool guess(const array<uint32_t, PEG_SLOTS> &pegs, uint32_t &black_keys, uint32_t &white_keys) {
        // Remember the positions of both order and color(number) mactched hidden pegs
        bool black_matched[PEG_SLOTS]{};
        // Remember the positions of only color(number) mactched hidden pegs
        bool white_matched[PEG_SLOTS]{};
        black_keys = 0;
        white_keys = 0;
        --m_tries_allowed;

        // Find black keys
        for (uint32_t i = 0; i < PEG_SLOTS; ++i) {
            if (pegs[i] == m_hidden_pegs[i]) {
                ++black_keys;
                black_matched[i] = true;
            }
        }

        // Find white keys
        for (uint32_t i = 0; i < PEG_SLOTS; ++i) {
            if (black_matched[i]) {
                continue;
            }
            for (uint32_t j = 0; j < PEG_SLOTS; ++j) {
                if (black_matched[j] || white_matched[j]) {
                    continue;
                }
                if (pegs[i] == m_hidden_pegs[j]) {
                    ++white_keys;
                    white_matched[j] = true;
                    break;
                }
            }
        }

        return (black_keys == PEG_SLOTS) ? true : false;
    }

    /**
     * @brief check if can go on guess
     * @return ture -- max trying turns are reached.
     */
    uint32_t turnsLeft() const {
        return m_tries_allowed;
    }

    /**
     * @brief get the hidden pegs
     * @param[inout] pegs -- return the pegs
     */
    void peer(array<uint32_t, PEG_SLOTS> &pegs) const {
        for (uint32_t i = 0; i < PEG_SLOTS; ++i) {
            pegs[i] = m_hidden_pegs[i];
        }
    }

private:
    uint32_t m_hidden_pegs[PEG_SLOTS];
    uint32_t m_peg_colors;
    mutable uint32_t m_tries_allowed;
};

int main(int argc, char *argv[])
{
    if ((argc == 2) && (strncmp(argv[1], "-h", 2) == 0)) {
        cout << "Usage: mastermind [OPTION]" << endl;
        cout << "Options: " << endl;
        cout << " -c NUMBER            [0 --> NUMBER) are the numbers used as color peg,"
             <<  "it should be great than 1 and less than 10, the default number is 8" << endl;
        cout << " -t TRIES_ALLOWED     How many turns are allowed to try, "
             << "it should be great than 1, the default value is 10" << endl;

        return 0;
    }

    uint32_t peg_colors{8};
    uint32_t tries_allowed{10};
    if ((argc == 5) || (argc == 3)) {
        // Skip the argv[0], it is program's name
        for (int i = 1; i < argc; ++i) {
            if (strncmp(argv[i], "-c", 2) == 0) {
                peg_colors = atoi(argv[++i]);
            } else if (strncmp(argv[i], "-t", 2) == 0) {
                tries_allowed = atoi(argv[++i]);
            }
        }
    }

    if (peg_colors > MAX_PEG_COLORS) {
        cout << "Wrong number of colors,  set to " << MAX_PEG_COLORS << endl;
        peg_colors = MAX_PEG_COLORS;
    }
    if (peg_colors < MIN_PEG_COLORS) {
        cout << "Wrong number of colors, set to " << MIN_PEG_COLORS << endl;
        peg_colors = MIN_PEG_COLORS;
    }
    if (tries_allowed < MIN_TRIES_ALLOWED) {
        cout << "Wrong turns, set to " << MIN_TRIES_ALLOWED << endl;
        tries_allowed = MIN_TRIES_ALLOWED;
    }

    cout << "The game is starting. You can try " << tries_allowed << " turns to guess the "
         << PEG_SLOTS << " hidden numbers." << endl;
    cout << "Each hidden number is from 0 to " << peg_colors - 1 << "." << endl;

    array<uint32_t, PEG_SLOTS> guess{};
    uint32_t black_keys{};
    uint32_t white_keys{};
    Mastermind game(peg_colors, tries_allowed);

    while (game.turnsLeft() > 0) {
        cout << "Please input " << PEG_SLOTS << " numbers[0 -- " << peg_colors - 1
             << "] separated by whitespace: ";
        for (uint32_t i = 0; i < PEG_SLOTS; ++i) {
            cin >> guess[i];
        }
        if (game.guess(guess, black_keys, white_keys)) {
            cout << "Congratulations! You win!" << endl;
            return 0;
        }
        cout << "Black keys: " << black_keys << ", White keys: " << white_keys << endl;
    }

    cout << "Sorry! You lost!" << endl;
    cout << "The hidden pegs:";
    game.peer(guess);
    for (auto a : guess) {
        cout << " " << a;
    }
    cout << endl;
    return 0;
}
