#ifndef Player_H
#define Player_H

namespace XO
{
    class TicTacToe;
    class player
    {
    private:
        // Pointer to the TicTacToe game to access API methods
        TicTacToe *game;

    public:
        player(TicTacToe *API) : game(API) {}
        void Move();
    };

}

#endif