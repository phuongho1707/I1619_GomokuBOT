#ifndef __GOMOKU_BOARD_H__
#define __GOMOKU_BOARD_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>

/* BLACK is the one who plays first. */
enum SIDE
{
    EMPTY = 0,
    BLACK = 67,
    WHITE = 63,
    WEVER = 155
};

/* Board's cell. */
struct cell
{
    size_t x, y;
    cell () {}
    cell (size_t _x, size_t _y): x(_x), y(_y) {}
    bool operator== (cell t)
    {
        return (x == t.x) && (y == t.y);
    }
    bool operator!= (cell t)
    {
        return !(*this == t);
    }
};

/* A pre-implemented gomoku board for computer player.
   All rows and columns are numbering from 0.
*/
class gomoku_board
{
private:
    std::vector<std::vector<uint8_t>> b;
    cell last;

public:
    /* Construct a board. */
    gomoku_board (size_t h = 15, size_t v = 15): b(std::vector<std::vector<uint8_t>>(h, std::vector<uint8_t>(v, EMPTY))) {}

    /* Construct a board using std::vector<std::vector<uint8_t>>. */
    gomoku_board (std::vector<std::vector<uint8_t>> _b): b(_b)
    {
        size_t msize = 0;
        for (size_t i = 0; i < b.size(); ++i)
            if (b[i].size() > msize)
                msize = b[i].size();
        for (size_t i = 0; i < b.size(); ++i)
            while (b[i].size() < msize)
                b[i].push_back(EMPTY);
    }

    /* Clear the board. */
    void clear ()
    {
        for (size_t i = 0; i < b.size(); ++i)
            for (size_t j = 0; j < b[i].size(); ++j)
                b[i][j] = EMPTY;
    }

    /* Play a move.
       This will NOT check for illegal moves. The GUI should do this instead.
    */
    void move (cell w, SIDE side)
    {
        b[w.x][w.y] = side;
        last = w;
    }

    /* Get last move.
       This helps separate the process of the GUI and the engine.
       GUI now does not have to care about the moves.
       And so the main function of the engine now does not have to include 2 parameters more.
    */
    cell lastmove ()
    {
        return last;
    }

    /* An operator to directly change a position of the board.
       It returns a pointer to the row in type std::vector<uint8_t>.
    */
    std::vector<uint8_t>& operator[] (size_t r)
    {
        return b[r];
    }

    /* Return ID of the last cell of the board.
       Used for determining the size of the board.
       ---
       If the board is empty, cell(INT_MAX, INT_MAX) will be returned.
    */
    cell lastcell ()
    {
        if (b.empty())
            return cell(INT_MAX, INT_MAX);
        return cell ((int)b.size() - 1, (int)b[0].size() - 1);
    }

    /* Is the board empty or not? */
    bool empty ()
    {
        return b.empty();
    }

    /* Get subboard.
       3 parameters: the starting cell, vertical length and horizontal length of the subboard.
       If 'length' parameters are positive, the subboard will be to the lower right corner of the board.
       If only 'vertical length' parameter is negative, the subboard will be to the lower left corner of the board.
       If only 'horizontal length' parameter is negative, the subboard will be to the upper right corner of the board.
       If 'length' parameters are negative, the subboard will be to the upper left corner of the board.
       The function returns a gomoku_board with fixed size.
       ---
       If some parameters are invalid, an exception will be returned.
    */
    gomoku_board subboard (cell sp, int hl, int vl)
    {
        gomoku_board p(abs(hl), abs(vl));
        int u = (hl >= 0 ? 0 : (abs(hl) - 1));
        for (int i = sp.x; i < (int)sp.x + hl; i = i + (hl >= 0) - (hl < 0), u = u + (hl >= 0) - (hl < 0))
        {
            int v = (vl >= 0 ? 0 : (abs(vl) - 1));
            for (int j = sp.y; j < (int)sp.y + vl; j = j + (vl >= 0) - (vl < 0), v = v + (vl >= 0) - (vl < 0))
                p[u][v] = b[i][j];
        }
        return p;
    }

    /* Compare two boards. */
    bool operator== (gomoku_board p)
    {
        if (lastcell() != p.lastcell())
            return 0;
        for (size_t i = 0; i <= lastcell().x; ++i)
            for (size_t j = 0; j <= lastcell().y; ++j)
            {
                if (b[i][j] == WEVER || p[i][j] == WEVER)
                    continue;
                if (b[i][j] != p[i][j])
                    return 0;
            }
        return 1;
    }
    bool operator!= (gomoku_board p)
    {
        return !(*this == p);
    }

    /* Rotate the board 90 degrees to the right. */
    void rotate ()
    {
        int m = lastcell().x + 1,
            n = lastcell().y + 1;
        gomoku_board p(n, m);
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                p[j][m - i - 1] = b[i][j];
        *this = p;
    }

    /* Check if gomoku_board p exists in *this.
       Specifically, only if any subboard with b's size exists in *this, the function returns the subboard.
       The returned subboard may differ from the given board in direction because it is the direction of
       the subboard that was firstly found in the board.
       If the function cannot find your given board, it returns an empty board.
    */
    gomoku_board exist (gomoku_board p)
    {
        for (size_t i = 0; i <= (lastcell().x - p.lastcell().x); ++i)
            for (size_t j = 0; j <= (lastcell().y - p.lastcell().y); ++j)
                for (int k = 0; k < 4; ++k)
                {
                    if (subboard(cell(i, j), p.lastcell().x + 1, p.lastcell().y + 1) == p)
                    {
                        for (size_t u = 0; u <= p.lastcell().x; ++u)
                            for (size_t v = 0; v <= (j + p.lastcell().y); ++v)
                                if (p[u][v] == WEVER)
                                    p[u][v] = b[u + i][v + j];
                        return p;
                    }
                    p.rotate();
                }
        return gomoku_board(0, 0);
    }

    /* Replace a subboard (that was firstly found in the board) with a board of the same size.
       This function will NOT do anything if the given subboard does not exist in the board.
       This function will NOT do anything if the sizes are different.
       This function will NOT rotate to find the board in all 4 directions.
    */
    void replace (gomoku_board p1, gomoku_board p2)
    {
        if (p1.lastcell() != p2.lastcell())
            return;
        cell pos;
        bool cont = 1;
        for (size_t i = 0; i <= (lastcell().x - p1.lastcell().x) && cont; ++i)
            for (size_t j = 0; j <= (lastcell().y - p1.lastcell().y) && cont; ++j)
                if (subboard(cell(i, j), p1.lastcell().x + 1, p1.lastcell().y + 1) == p1)
                {
                    pos = cell(i, j);
                    cont = 0;
                    break;
                }
        if (cont)
            return;
        for (size_t i = pos.x; i <= (pos.x + p1.lastcell().x); ++i)
            for (size_t j = pos.y; j <= (pos.y + p1.lastcell().y); ++j)
                b[i][j] = p2[i - pos.x][j - pos.y];
    }

    /* Get a std::vector of cells that are different between the 2 boards.
       If the 2 boards are different in size, the function returns an empty std::vector.
    */
    std::vector<cell> diff (gomoku_board p)
    {
        if (lastcell() != p.lastcell())
            return std::vector<cell>();
        std::vector<cell> rep;
        for (size_t i = 0; i <= lastcell().x; ++i)
            for (size_t j = 0; j <= lastcell().y; ++j)
                if (b[i][j] != p[i][j] && b[i][j] != WEVER && p[i][j] != WEVER)
                    rep.push_back(cell(i, j));
        return rep;
    }
};

#endif /* __GOMOKU_BOARD_H__ */
