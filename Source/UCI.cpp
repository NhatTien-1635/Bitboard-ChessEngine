//
// Created by Hi on 4/30/2026.
//

#include "../Header/UCI.h"


void UCI::InitializeEngine() {
    MoveGenerator::InitGenerator();
    Evaluator::InitializeEvaluationTable();
    TranspositionTable::InitHashTable();
    Engine::InitTableLMR();
}

void UCI::RunLoop() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.find("quit") != std::string::npos) {
            Limits::stop_flag = true;
            while (Limits::searching_flag) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            break;
        }

        ParseCommand(line);
    }
}

void UCI::ParseCommand(const std::string &line) {
    if (line.find("ucinewgame") != std::string::npos) {
        Engine::ClearHashTable();
        Evaluator::ClearHistoryKillerMoveTable();
        ParsePosition("position startpos");
        return;
    }

    if (line.find("uci") != std::string::npos) {
        std::cout << "id name PotatoBotNo13\n"
                "id author NhatTien-1635\n"
                "uciok" << std::endl;
        return;
    }

    if (line.find("isready") != std::string::npos) {
        std::cout << "readyok" << std::endl;
        return;
    }

    if (line.find("position") != std::string::npos) {
        ParsePosition(line);
        return;
    }

    if (line.find("go") != std::string::npos) {
        ParseGo(line);
        return;
    }

    if (line.find("stop") != std::string::npos) {
        Limits::stop_flag = true;
        Limits::searching_flag = false;
        return;
    }
}

void UCI::ParsePosition(const std::string &line) {
    std::stringstream ss(line);
    std::string token;

    ss >> token; //"position"
    ss >> token; //fen/startpos

    if (token == "startpos") {
        chess_board.ParsePositionFromFEN(FEN_STARTING_POSITION);
        ss >> token;
    } else if (token == "fen") {
        std::string fen = "";

        while (ss >> token && token != "moves") {
            fen += (fen.empty() ? "" : " ") + token;
        }

        chess_board.ParsePositionFromFEN(fen);
    }

    if (token == "moves") {
        std::string move_token;
        while (ss >> move_token) {
            int move = ParseMove(move_token);
            chess_board.MakeMove(move);
        }
    }

    // chess_board.PrintBoard();
}

UCI::UCI() {
    InitializeEngine();
    chess_board.ParsePositionFromFEN(FEN_STARTING_POSITION);
}

UCI::~UCI() {
}

//Parse move from input (ex: e2e4, e7e8q,...)
int UCI::ParseMove(const std::string &token) {
    int source_square = (int) (token.at(0) - 'a') + (8 - (int) (token.at(1) - '0')) * 8;
    int target_square = (int) (token.at(2) - 'a') + (8 - (int) (token.at(3) - '0')) * 8;

    MoveList move_list;
    chess_board.PopulateMoveList(move_list);

    int move = MoveList::invalid_move;
    int move_count = move_list.GetMoveCount();
    for (int index = 0; index < move_count; ++index) {
        move = move_list.GetMove(index);

        if (MoveList::DecodeGetSourceSquare(move) != source_square || MoveList::DecodeGetTargetSquare(move) !=
            target_square) {
            continue;
        }

        //No promotion
        Piece promotion_move = MoveList::DecodeGetPromotedPiece(move);
        if (token.length() == 4 && promotion_move == NoPiece) {
            return move;
        }

        if (token.length() > 4) {
            //Checking promotion
            char promoted_piece = token.at(4);

            //Promoted to a knight
            if (promoted_piece == 'n' && (promotion_move == WhiteKnight || promotion_move == BlackKnight)) {
                return move;
            }

            //Promoted to a bishop
            if (promoted_piece == 'b' && (promotion_move == WhiteBishop || promotion_move == BlackBishop)) {
                return move;
            }

            //Promoted to a rook
            if (promoted_piece == 'r' && (promotion_move == WhiteRook || promotion_move == BlackRook)) {
                return move;
            }

            //Promoted to a queen
            if (promoted_piece == 'q' && (promotion_move == WhiteQueen || promotion_move == BlackQueen)) {
                return move;
            }
        }
    }

    //Somehow we got here :D?
    std::cerr << "ParseMove -> token '" << token << "' input is an illegal move!" << std::endl;
    return MoveList::invalid_move;
}

void UCI::ParseGo(const std::string &line) {
    //Reset everything
    Limits::stop_flag = false;
    Limits::node_count = 0;
    Limits::searching_flag = true;
    Limits::use_time_management = false;
    Limits::start_time = Limits::GetTimeMs();
    Limits::infinite = false;

    //Initialization
    std::istringstream iss(line);
    std::string token;

    int depth = Engine::max_ply;
    int64_t white_time = -1;
    int64_t black_time = -1;
    int64_t white_increment = 0;
    int64_t black_increment = 0;

    //Remove the 'go' token
    iss >> token; //token == "go"

    //Loop over the command
    while (iss >> token) {
        if (token == "wtime") iss >> white_time;
        if (token == "btime") iss >> black_time;
        if (token == "winc") iss >> white_increment;
        if (token == "binc") iss >> black_increment;
        if (token == "depth") iss >> depth;
        if (token == "movetime") {
            iss >> Limits::time_limit;
            Limits::use_time_management = true;
        }
        if (token == "infinite") Limits::infinite = true;
    }

    //Calculate move time
    if (!Limits::infinite && !Limits::use_time_management) {
        int my_time = chess_board.CurrentSide() == White ? white_time : black_time;
        int increment_time = chess_board.CurrentSide() == White ? white_increment : black_increment;

        if (my_time != -1) {
            Limits::use_time_management = true;
            Limits::time_limit = (my_time / 40) + (increment_time / 2) - 50; //-50ms is for lag

            //Hard cap at 5s
            if (Limits::time_limit > 5000) {
                Limits::time_limit = 5000;
            }

            //Hard cap at 0.02 ms
            if (Limits::time_limit < 20) {
                Limits::time_limit = 20;
            }
        }
    }

    //Start searching best move in a different thread
    std::thread search_thread([this, depth]() {
        int best_move = Engine::GetBestMove(this->chess_board, depth);

        std::string returning_string = "bestmove  " + Limits::GetMoveString(best_move);

        Piece promoted_piece = MoveList::DecodeGetPromotedPiece(best_move);
        if (promoted_piece != NoPiece) {
            if (promoted_piece == WhiteKnight || promoted_piece == BlackKnight) {
                returning_string += "n";
            }
            if (promoted_piece == WhiteBishop || promoted_piece == BlackBishop) {
                returning_string += "b";
            }
            if (promoted_piece == WhiteRook || promoted_piece == BlackRook) {
                returning_string += "r";
            }
            if (promoted_piece == WhiteQueen || promoted_piece == BlackQueen) {
                returning_string += "q";
            }
        }

        std::cout << returning_string << std::endl;

        Limits::searching_flag = false;
    });

    search_thread.detach();
}

void UCI::PrintBoard() const {
    chess_board.PrintBoard();
}
