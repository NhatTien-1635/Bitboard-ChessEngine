//
// Created by Hi on 3/23/2026.
//

#include "../Header/ChessBoard.h"

void ChessBoard::PrintBoard() const {
    std::cout << std::setw(4) << "   a   b   c   d   e   f   g   h\n";
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            std::cout << (8 - (i / 8));
        }

        std::cout << std::setw(3) << PieceToChar(mailbox[i]) << ' ';
        if (i % 8 == 7) {
            std::cout << '\n';
        }
    }

    std::cout << "===============================\n";

    std::string print_val;

    //Printing Side
    if (board_state.side_to_move == Both) {
        print_val = "Both (How tf)";
    } else if (board_state.side_to_move == White) {
        print_val = "White";
    } else {
        print_val = "Black";
    }
    std::cout << "Side to move: " << print_val << '\n';

    //Print castling
    print_val.clear();
    if (!board_state.castling_right) {
        print_val = "Both side can't castle\n";
    } else {
        if (board_state.castling_right & WhiteToKing) {
            print_val += "- White castle king side\n";
        }
        if (board_state.castling_right & WhiteToQueen) {
            print_val += "- White castle queen side\n";
        }
        if (board_state.castling_right & BlackToKing) {
            print_val += "- Black castle king side\n";
        }
        if (board_state.castling_right & BlackToQueen) {
            print_val += "- Black castle queen side\n";
        }
    }
    std::cout << "Castling right:\n" << print_val;

    //Print en passant
    print_val.clear();
    if (board_state.en_passant == NoSquare) {
        print_val = "No en passant";
    } else {
        print_val += MoveGenerator::SquareToString(board_state.en_passant);
    }
    std::cout << "En passant square: " << print_val << '\n';
}

void ChessBoard::PrintAttack(Side side) const {
    std::cout << std::setw(4) << "   a   b   c   d   e   f   g   h\n";
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            std::cout << (8 - (i / 8));
        }

        std::cout << std::setw(3) << (IsSquaredAttacked(i, side) ? '1' : '.') << ' ';
        if (i % 8 == 7) {
            std::cout << '\n';
        }
    }

    std::cout << "===============================\n";
}

void ChessBoard::ParsePositionFromFEN(std::string_view position) {
    ClearBoard();

    //Update mailbox & piece_bitboard
    int str_index = 0;
    int square = 0;
    while (square < 64) {
        char piece = position.at(str_index);
        if (piece == '/') {
            ++str_index;
            continue;
        }

        if (isdigit(piece)) {
            int empty_num = (int) (piece - '0');
            for (int i = 0; i < empty_num; ++i) {
                mailbox[square++] = NoPiece;
            }
        } else {
            mailbox[square] = CharToPieceIndex(piece);
            piece_bitboard[mailbox[square]] |= (1ULL << square);
            ++square;
        }
        ++str_index;
    }

    //Update white occupancy board
    for (int i = 0; i < 6; ++i) {
        occupancy_bitboard[White] |= piece_bitboard[i];
    }

    //Update black occupancy board
    for (int i = 6; i < 12; ++i) {
        occupancy_bitboard[Black] |= piece_bitboard[i];
    }

    occupancy_bitboard[Both] = occupancy_bitboard[Black] | occupancy_bitboard[White];

    //Update turn
    char fen_turn = position.at(++str_index);
    board_state.side_to_move = (fen_turn == 'w' ? White : Black);

    //Update castling right
    str_index += 2; //Move to castling right portion
    char rights = position.at(str_index);
    while (rights != ' ') {
        switch (rights) {
            case 'K': board_state.castling_right |= WhiteToKing;
                break;
            case 'Q': board_state.castling_right |= WhiteToQueen;
                break;
            case 'k': board_state.castling_right |= BlackToKing;
                break;
            case 'q': board_state.castling_right |= BlackToQueen;
                break;
            case '-': break;
            default:
                break;
        }
        rights = position.at(++str_index);
    }

    //Update en passant
    char en_pos = position.at(++str_index);
    if (en_pos == '-') {
        ++str_index;
    } else {
        int file = en_pos - 'a';
        int rank = 8 - (position.at(++str_index) - '0');
        board_state.en_passant = rank * 8 + file;
        ++str_index;
    }

    // Skip the space after en_passant
    while (str_index < position.length() && position.at(str_index) == ' ') {
        ++str_index;
    }

    // Update half-time (Half-clock)
    int half_move = 0;
    while (str_index < position.length() && position.at(str_index) != ' ') {
        half_move = half_move * 10 + (position.at(str_index) - '0');
        ++str_index;
    }
    board_state.half_clock = half_move; // Adjust this variable name if needed

    // Skip the space after half-clock
    while (str_index < position.length() && position.at(str_index) == ' ') {
        ++str_index;
    }

    // Update turn count
    int full_move = 0;
    while (str_index < position.length() && position.at(str_index) != ' ') {
        full_move = full_move * 10 + (position.at(str_index) - '0');
        ++str_index;
    }
    board_state.turn_count = full_move;
}

bool ChessBoard::MakeMove(int encoded_move) {
    //Reserve board state
    history[game_ply++] = board_state;

    Square source_square = MoveList::DecodeGetSourceSquare(encoded_move);
    Square target_square = MoveList::DecodeGetTargetSquare(encoded_move);
    Piece piece = MoveList::DecodeGetPiece(encoded_move);

    //Quiet move
    piece_bitboard[piece].InsertBit(target_square);
    occupancy_bitboard[board_state.side_to_move].InsertBit(target_square);
    mailbox[target_square] = piece;
    TranspositionTable::UpdatePiece(piece, target_square);

    piece_bitboard[piece].RemoveBit(source_square);
    occupancy_bitboard[board_state.side_to_move].RemoveBit(source_square);
    mailbox[source_square] = NoPiece;
    TranspositionTable::UpdatePiece(piece, source_square);

    //Capture
    Piece capture_piece = MoveList::DecodeGetCapturePiece(encoded_move);
    bool en_passant = MoveList::DecodeGetEnpassantFlag(encoded_move);
    if (capture_piece != NoPiece && !en_passant) {
        piece_bitboard[capture_piece].RemoveBit(target_square);
        occupancy_bitboard[opponent_side[board_state.side_to_move]].RemoveBit(target_square);
        TranspositionTable::UpdatePiece(capture_piece, target_square);
    }

    //En passant
    if (en_passant) {
        Side opponent = opponent_side[board_state.side_to_move];
        Piece pawn = Piece(opponent * 6 + WhitePawn);

        piece_bitboard[pawn].RemoveBit(target_square + forward_pawn_offset[opponent]);
        occupancy_bitboard[opponent].RemoveBit(target_square + forward_pawn_offset[opponent]);
        mailbox[target_square + forward_pawn_offset[opponent]] = NoPiece;
        TranspositionTable::UpdatePiece(pawn, target_square + forward_pawn_offset[opponent]);
    }

    //Promotion
    Piece promoted_piece = MoveList::DecodeGetPromotedPiece(encoded_move);
    if (promoted_piece != NoPiece) {
        piece_bitboard[piece].RemoveBit(target_square);
        TranspositionTable::UpdatePiece(piece, target_square);

        piece_bitboard[promoted_piece].InsertBit(target_square);
        TranspositionTable::UpdatePiece(promoted_piece, target_square);

        mailbox[target_square] = promoted_piece;
    }


    //Remove enpassant key;
    if (board_state.en_passant != NoSquare) {
        TranspositionTable::UpdateEnpassant(board_state.en_passant);
    }

    //Reset enpassant
    board_state.en_passant = NoSquare;

    //Double
    bool double_push = MoveList::DecodeGetDoublePushFlag(encoded_move);
    if (double_push) {
        board_state.en_passant = (target_square + forward_pawn_offset[opponent_side[board_state.side_to_move]]);
        TranspositionTable::UpdateEnpassant(target_square + forward_pawn_offset[opponent_side[board_state.side_to_move]]);
    }

    //Castle
    bool castling = MoveList::DecodeGetCastlingFlag(encoded_move);
    if (castling) {
        int rook_source = 0;
        int rook_target = 0;

        switch (target_square) {
            //White castle queen side
            case c1:
                rook_source = a1;
                rook_target = d1;
                break;
            //White castle king side
            case g1:
                rook_source = h1;
                rook_target = f1;
                break;
            //Black castle queen side
            case c8:
                rook_source = a8;
                rook_target = d8;
                break;
            //Black castle king side
            case g8:
                rook_source = h8;
                rook_target = f8;
                break;

            default:
                break;
        }

        Side side = (Side) board_state.side_to_move;
        Piece rook = Piece(side * 6 + WhiteRook);

        piece_bitboard[rook].RemoveBit(rook_source);
        piece_bitboard[rook].InsertBit(rook_target);

        occupancy_bitboard[side].RemoveBit(rook_source);
        occupancy_bitboard[side].InsertBit(rook_target);

        mailbox[rook_source] = NoPiece;
        mailbox[rook_target] = rook;

        TranspositionTable::UpdatePiece(rook, rook_source);
        TranspositionTable::UpdatePiece(rook, rook_target);
    }

    //Update occupancy bitboard
    occupancy_bitboard[Both] = (occupancy_bitboard[White] | occupancy_bitboard[Black]);

    //Update side to move
    board_state.side_to_move = opponent_side[board_state.side_to_move];
    TranspositionTable::UpdateSide();

    //Update castling
    TranspositionTable::UpdateCastling(board_state.castling_right);
    board_state.castling_right &= (castling_right_mask[source_square] & castling_right_mask[target_square]);
    TranspositionTable::UpdateCastling(board_state.castling_right);

    //Update hash key
    board_state.hash_key = TranspositionTable::GetKey();

    //Debug stuff
#ifdef HASH_KEY_DEBUG
    uint64_t scratch_key = TranspositionTable::GenerateKey(*this);
    if (board_state.hash_key != scratch_key) {
        std::cout << "Move: " << MoveGenerator::SquareToString(source_square) <<
                MoveGenerator::SquareToString(target_square) << '\n';
        PrintBoard();
        std::cout << "Current key: " << board_state.hash_key << '\n';
        std::cout << "Correct key: " << scratch_key << '\n';
        std::cin.get();
    }
#endif

    //Update turn count
    if (board_state.side_to_move == White) {
        ++board_state.turn_count;
    }

    //Update half-clock
    ++board_state.half_clock;
    if (capture_piece != NoPiece || piece == BlackPawn || piece == WhitePawn) {
        board_state.half_clock = 0;
    }

    if (board_state.half_clock == 100) {
        // return false;
    }

    //King in check
    if (IsSquaredAttacked(piece_bitboard[history[game_ply - 1].side_to_move * 6 + WhiteKing].GetFirstLSBIndex(),
                          opponent_side[history[game_ply - 1].side_to_move])) {
        UnmakeMove(encoded_move);
        return false;
    }

    return true;
}

bool ChessBoard::MakeCaptureMove(int encoded_move) {
    if (MoveList::DecodeGetCapturePiece(encoded_move) != NoPiece) {
        return MakeMove(encoded_move);
    }
    //Move is not capture move
    else {
        return false;
    }
}

void ChessBoard::UnmakeMove(int encoded_move) {
    //Restore game state
    board_state = std::move(history[--game_ply]);
    TranspositionTable::SetKey(board_state.hash_key);

    Square source_square = MoveList::DecodeGetSourceSquare(encoded_move);
    Square target_square = MoveList::DecodeGetTargetSquare(encoded_move);
    Piece piece = MoveList::DecodeGetPiece(encoded_move);

    //Quiet move
    piece_bitboard[piece].RemoveBit(target_square);
    occupancy_bitboard[board_state.side_to_move].RemoveBit(target_square);

    piece_bitboard[piece].InsertBit(source_square);
    occupancy_bitboard[board_state.side_to_move].InsertBit(source_square);

    mailbox[target_square] = NoPiece;
    mailbox[source_square] = piece;

    //Capture
    Piece captured_piece = MoveList::DecodeGetCapturePiece(encoded_move);
    bool en_passant = MoveList::DecodeGetEnpassantFlag(encoded_move);
    if ((captured_piece != NoPiece) && !en_passant) {
        piece_bitboard[captured_piece].InsertBit(target_square);
        occupancy_bitboard[opponent_side[board_state.side_to_move]].InsertBit(target_square);

        mailbox[target_square] = captured_piece;
    }

    //Promotion
    Piece promoted_piece = MoveList::DecodeGetPromotedPiece(encoded_move);
    if (promoted_piece != NoPiece) {
        piece_bitboard[promoted_piece].RemoveBit(target_square);
    }

    //Double push -> There's nothing to do

    //En passant
    if (en_passant) {
        Side opponent = opponent_side[board_state.side_to_move];
        Piece opponent_pawn = Piece(opponent_side[board_state.side_to_move] * 6 + WhitePawn);

        piece_bitboard[opponent_pawn].InsertBit(target_square + forward_pawn_offset[opponent]);
        occupancy_bitboard[opponent].InsertBit(target_square + forward_pawn_offset[opponent]);
        mailbox[target_square + forward_pawn_offset[opponent]] = opponent_pawn;
    }

    //Castling
    bool castling = MoveList::DecodeGetCastlingFlag(encoded_move);
    if (castling) {
        int rook_source = 0;
        int rook_target = 0;

        switch (target_square) {
            //White castle queen side
            case c1:
                rook_source = d1;
                rook_target = a1;
                break;
            //White castle king side
            case g1:
                rook_source = f1;
                rook_target = h1;
                break;
            //Black castle queen side
            case c8:
                rook_source = d8;
                rook_target = a8;
                break;
            //Black castle king side
            case g8:
                rook_source = f8;
                rook_target = h8;
                break;

            default:
                break;
        }

        Side side = (Side) board_state.side_to_move;
        Piece rook = Piece(side * 6 + WhiteRook);

        piece_bitboard[rook].RemoveBit(rook_source);
        piece_bitboard[rook].InsertBit(rook_target);

        occupancy_bitboard[side].RemoveBit(rook_source);
        occupancy_bitboard[side].InsertBit(rook_target);

        mailbox[rook_source] = NoPiece;
        mailbox[rook_target] = rook;
    }

    //Restore occupancy bitboard
    occupancy_bitboard[Both] = (occupancy_bitboard[White] | occupancy_bitboard[Black]);
}

//Check if the square is attacked by the attacker Side
bool ChessBoard::IsSquaredAttacked(uint8_t square, uint8_t attacker) const {
    //Calculate offset
    int offset = attacker * 6;

    //Checking pawn attack
    Side victim_side = opponent_side[attacker];
    if (MoveGenerator::GetPawnAttack(square, victim_side) & piece_bitboard[offset + WhitePawn]) return true;

    //Checking knight attack
    if (MoveGenerator::GetKnightAttack(square) & piece_bitboard[offset + WhiteKnight]) return true;

    //Checking sliding attacks
    if (MoveGenerator::GetBishopAttack(square, occupancy_bitboard[Both]) & (piece_bitboard[offset + WhiteBishop] |
                                                                            piece_bitboard[offset + WhiteQueen]))
        return true;
    if (MoveGenerator::GetRookAttack(square, occupancy_bitboard[Both]) & (piece_bitboard[offset + WhiteRook] |
                                                                          piece_bitboard[offset + WhiteQueen]))
        return true;

    //Checking king attack
    if (MoveGenerator::GetKingAttack(square) & piece_bitboard[offset + WhiteKing]) return true;

    return false;
}

int ChessBoard::GetWeakestAttackerSquare(uint8_t square, uint8_t attacker, const Bitmap &occupancy) const {
    int offset = 6 * attacker;
    Bitmap attack;

    Side victim_side = opponent_side[attacker];
    attack = (MoveGenerator::GetPawnAttack(square, victim_side) & piece_bitboard[offset + WhitePawn] & occupancy);
    if (attack) return attack.GetFirstLSBIndex();

    attack = (MoveGenerator::GetKnightAttack(square) & piece_bitboard[offset + WhiteKnight] & occupancy);
    if (attack) return attack.GetFirstLSBIndex();

    attack = (MoveGenerator::GetBishopAttack(square, occupancy) & piece_bitboard[offset + WhiteBishop] & occupancy);
    if (attack) return attack.GetFirstLSBIndex();

    attack = (MoveGenerator::GetRookAttack(square, occupancy) & piece_bitboard[offset + WhiteRook] & occupancy);
    if (attack) return attack.GetFirstLSBIndex();

    attack = (MoveGenerator::GetQueenAttack(square, occupancy) & piece_bitboard[offset + WhiteQueen] & occupancy);
    if (attack) return attack.GetFirstLSBIndex();

    attack = (MoveGenerator::GetKingAttack(square) & piece_bitboard[offset + WhiteKing]);
    if (attack) return attack.GetFirstLSBIndex();

    return NoSquare;
}

ChessBoard::ChessBoard(const std::string_view &FEN) {
    ParsePositionFromFEN(FEN);
    board_state.hash_key = TranspositionTable::GenerateKey(*this);
    TranspositionTable::SetKey(TranspositionTable::GenerateKey(*this));
}

void ChessBoard::ClearBoard() {
    for (int i = 0; i < 64; ++i) {
        mailbox[i] = NoPiece;
    }

    for (int i = 0; i < 12; ++i) {
        piece_bitboard[i] = 0ULL;
    }

    for (int i = 0; i < 3; ++i) {
        occupancy_bitboard[i] = 0ULL;
    }

    board_state.side_to_move = Both;
    board_state.en_passant = NoSquare;
    board_state.castling_right = (CastlingRight) 0;
    board_state.turn_count = 0;
    board_state.half_clock = 0;
}

void ChessBoard::PopulateMoveList(MoveList &move_list) {
    if (board_state.side_to_move == White) {
        PopulateMove<White>(move_list);
    } else {
        PopulateMove<Black>(move_list);
    }
}

void ChessBoard::PopulateCaptureMoveList(MoveList &move_list) {
    if (board_state.side_to_move == White) {
        PopulateCaptureMove<White>(move_list);
    } else {
        PopulateCaptureMove<Black>(move_list);
    }
}

ChessBoard::ChessBoard() {
    ParsePositionFromFEN(FEN_STARTING_POSITION);
    board_state.hash_key = TranspositionTable::GenerateKey(*this);
    TranspositionTable::SetKey(TranspositionTable::GenerateKey(*this));
}

Piece ChessBoard::CharToPieceIndex(char ch) {
    switch (ch) {
        // White Pieces
        case 'P': return WhitePawn;
        case 'N': return WhiteKnight;
        case 'B': return WhiteBishop;
        case 'R': return WhiteRook;
        case 'Q': return WhiteQueen;
        case 'K': return WhiteKing;

        // Black Pieces
        case 'p': return BlackPawn;
        case 'n': return BlackKnight;
        case 'b': return BlackBishop;
        case 'r': return BlackRook;
        case 'q': return BlackQueen;
        case 'k': return BlackKing;

        // Default case for invalid characters
        default: return NoPiece;
    };
}

char ChessBoard::PieceToChar(int piece) {
    return piece_strings[piece];
}

template<Side side>
void ChessBoard::PopulateMove(MoveList &move_list) {
    constexpr Side opponent = opponent_side[side];

    int source_square = 0;
    int target_square = 0;

    //Pre-allocate mem
    Bitmap bitboard{};
    Bitmap move_bitboard{};
    Bitmap quiet_move{};
    Bitmap capture_move{};

    //King for not adding capture king move
    constexpr Piece own_king = (side == White) ? WhiteKing : BlackKing;
    constexpr Piece opponent_king = opposite_piece[own_king];

    //Pawn
    constexpr Piece own_pawn = (side == White) ? WhitePawn : BlackPawn;
    constexpr Piece opponent_pawn = opposite_piece[own_pawn];

    bitboard = piece_bitboard[own_pawn];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);
        target_square = source_square + forward_pawn_offset[side];

        if (mailbox[target_square] == NoPiece) {
            move_bitboard.InsertBit(target_square);

            //Double push
            if (((1ULL << source_square) & pawn_start_rank[side]) &&
                (mailbox[target_square + forward_pawn_offset[side]] == NoPiece)) {
                move_list.AddMove(source_square, target_square + forward_pawn_offset[side], own_pawn, NoPiece, NoPiece,
                                  true, false, false);
            }
        }

        //Getting pawn attack
        Bitmap pawn_attack = MoveGenerator::GetPawnAttack(source_square, side);

        //Masking attack
        move_bitboard |= (occupancy_bitboard[opponent] & pawn_attack & ~piece_bitboard[opponent_king]);

        //En passant
        if (board_state.en_passant != NoSquare && ((uint64_t) pawn_attack & (1ULL << board_state.en_passant))) {
            move_list.AddMove(source_square, board_state.en_passant, own_pawn, NoPiece, opponent_pawn, false, true,
                              false);
        }

        //Get promote target square
        Bitmap promotion_move = (uint64_t) move_bitboard & promotion_zone[side];

        //Get normal quiet move
        move_bitboard ^= promotion_move;

        //Normal quiet move
        while (move_bitboard) {
            target_square = move_bitboard.GetFirstLSBIndex();
            move_bitboard.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_pawn, NoPiece, mailbox[target_square], false, false,
                              false);
        }

        while (promotion_move) {
            target_square = promotion_move.GetFirstLSBIndex();
            promotion_move.RemoveBit(target_square);

            for (int piece = own_pawn + 1; piece < own_pawn + 5; ++piece) {
                move_list.AddMove(source_square, target_square, own_pawn, piece, mailbox[target_square], false, false,
                                  false);
            }
        }
    }

    //Knight
    constexpr Piece own_knight = (side == White) ? WhiteKnight : BlackKnight;
    bitboard = piece_bitboard[own_knight];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetKnightAttack(source_square)
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        quiet_move = move_bitboard ^ capture_move;

        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_knight, NoPiece, mailbox[target_square], false, false,
                              false);
        }

        while (quiet_move) {
            target_square = quiet_move.GetFirstLSBIndex();
            quiet_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_knight, NoPiece, NoPiece, false, false, false);
        }
    }

    // Bishop
    constexpr Piece own_bishop = (side == White) ? WhiteBishop : BlackBishop;
    bitboard = piece_bitboard[own_bishop];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetBishopAttack(source_square, occupancy_bitboard[Both])
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        quiet_move = move_bitboard ^ capture_move;
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_bishop, NoPiece, mailbox[target_square], false, false,
                              false);
        }

        while (quiet_move) {
            target_square = quiet_move.GetFirstLSBIndex();
            quiet_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_bishop, NoPiece, NoPiece, false, false, false);
        }
    }

    // Rook
    constexpr Piece own_rook = (side == White) ? WhiteRook : BlackRook;
    bitboard = piece_bitboard[own_rook];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetRookAttack(source_square, occupancy_bitboard[Both])
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        quiet_move = move_bitboard ^ capture_move;
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_rook, NoPiece, mailbox[target_square], false, false,
                              false);
        }

        while (quiet_move) {
            target_square = quiet_move.GetFirstLSBIndex();
            quiet_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_rook, NoPiece, NoPiece, false, false, false);
        }
    }

    // Queen
    constexpr Piece own_queen = (side == White) ? WhiteQueen : BlackQueen;
    bitboard = piece_bitboard[own_queen];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetQueenAttack(source_square, occupancy_bitboard[Both])
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        quiet_move = move_bitboard ^ capture_move;
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_queen, NoPiece, mailbox[target_square], false, false,
                              false);
        }

        while (quiet_move) {
            target_square = quiet_move.GetFirstLSBIndex();
            quiet_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_queen, NoPiece, NoPiece, false, false, false);
        }
    }

    //King
    bitboard = piece_bitboard[own_king];
    source_square = bitboard.GetFirstLSBIndex();

    move_bitboard = (MoveGenerator::GetKingAttack(source_square)
                     & ~occupancy_bitboard[side]
                     & ~piece_bitboard[opponent_king]);

    capture_move = move_bitboard & occupancy_bitboard[opponent];
    quiet_move = move_bitboard ^ capture_move;
    while (capture_move) {
        target_square = capture_move.GetFirstLSBIndex();
        capture_move.RemoveBit(target_square);

        move_list.AddMove(source_square, target_square, own_king, NoPiece, mailbox[target_square], false, false, false);
    }

    while (quiet_move) {
        target_square = quiet_move.GetFirstLSBIndex();
        quiet_move.RemoveBit(target_square);

        move_list.AddMove(source_square, target_square, own_king, NoPiece, NoPiece, false, false,
                          false);
    }

    //White castling
    if constexpr (side == White) {
        if (IsSquaredAttacked(source_square, Black)) {
            return;
        }

        if (board_state.castling_right & WhiteToQueen) {
            bool no_attack = (!IsSquaredAttacked(c1, Black) && !IsSquaredAttacked(d1, Black));
            bool no_occupancy = ((mailbox[b1] == NoPiece) && (mailbox[c1] == NoPiece) && (mailbox[d1] == NoPiece));
            bool rook_exist = (mailbox[a1] == WhiteRook);
            if (no_attack && no_occupancy && rook_exist) {
                move_list.AddMove(e1, c1, WhiteKing, NoPiece, NoPiece, false, false, true);
            }
        }

        if (board_state.castling_right & WhiteToKing) {
            bool no_attack = (!IsSquaredAttacked(f1, Black) && !IsSquaredAttacked(g1, Black));
            bool no_occupancy = ((mailbox[f1] == NoPiece) && (mailbox[g1] == NoPiece));
            bool rook_exist = (mailbox[h1] == WhiteRook);
            if (no_attack && no_occupancy && rook_exist) {
                move_list.AddMove(e1, g1, WhiteKing, NoPiece, NoPiece, false, false, true);
            }
        }
    }

    //Black castling
    else {
        //King in check
        if (IsSquaredAttacked(source_square, White)) {
            return;
        }

        if (board_state.castling_right & BlackToQueen) {
            bool no_attack = (!IsSquaredAttacked(c8, White) && !IsSquaredAttacked(d8, White));
            bool no_occupancy = (mailbox[b8] == NoPiece && (mailbox[c8] == NoPiece) && (mailbox[d8] == NoPiece));
            bool rook_exist = (mailbox[a8] == BlackRook);
            if (no_attack && no_occupancy && rook_exist) {
                move_list.AddMove(e8, c8, BlackKing, NoPiece, NoPiece, false, false, true);
            }
        }

        if (board_state.castling_right & BlackToKing) {
            bool no_attack = (!IsSquaredAttacked(f8, White) && !IsSquaredAttacked(g8, White));
            bool no_occupancy = ((mailbox[f8] == NoPiece) && (mailbox[g8] == NoPiece));
            bool rook_exist = (mailbox[h8] == BlackRook);
            if (no_attack && no_occupancy && rook_exist) {
                move_list.AddMove(e8, g8, BlackKing, NoPiece, NoPiece, false, false, true);
            }
        }
    }
}

template<Side side>
void ChessBoard::PopulateCaptureMove(MoveList &move_list) {
    constexpr Side opponent = opponent_side[side];

    int source_square = 0;
    int target_square = 0;

    //Pre-allocate mem
    Bitmap bitboard;
    Bitmap move_bitboard;
    Bitmap capture_move;

    //King for not adding capture king move
    constexpr Piece own_king = (side == White) ? WhiteKing : BlackKing;
    constexpr Piece opponent_king = opposite_piece[own_king];

    //Pawn
    constexpr Piece own_pawn = (side == White) ? WhitePawn : BlackPawn;
    constexpr Piece opponent_pawn = opposite_piece[own_pawn];

    bitboard = piece_bitboard[own_pawn];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        //Getting pawn attack
        Bitmap pawn_attack = MoveGenerator::GetPawnAttack(source_square, side);

        //Masking attack
        move_bitboard |= (occupancy_bitboard[opponent] & pawn_attack & ~piece_bitboard[opponent_king]);

        //En passant
        if (board_state.en_passant != NoSquare && ((uint64_t) pawn_attack & (1ULL << board_state.en_passant))) {
            move_list.AddMove(source_square, board_state.en_passant, own_pawn, NoPiece, opponent_pawn, false, true,
                              false);
        }

        //Get promote target square
        Bitmap promotion_move = (uint64_t) move_bitboard & promotion_zone[side];

        //Get normal quiet move
        move_bitboard ^= promotion_move;

        //Normal quiet move
        while (move_bitboard) {
            target_square = move_bitboard.GetFirstLSBIndex();
            move_bitboard.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_pawn, NoPiece, mailbox[target_square], false, false,
                              false);
        }

        while (promotion_move) {
            target_square = promotion_move.GetFirstLSBIndex();
            promotion_move.RemoveBit(target_square);

            for (int piece = own_pawn + 1; piece < own_pawn + 5; ++piece) {
                move_list.AddMove(source_square, target_square, own_pawn, piece, mailbox[target_square], false, false,
                                  false);
            }
        }
    }

    //Knight
    constexpr Piece own_knight = (side == White) ? WhiteKnight : BlackKnight;
    bitboard = piece_bitboard[own_knight];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetKnightAttack(source_square)
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_knight, NoPiece, mailbox[target_square], false, false,
                              false);
        }
    }

    // Bishop
    constexpr Piece own_bishop = (side == White) ? WhiteBishop : BlackBishop;
    bitboard = piece_bitboard[own_bishop];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetBishopAttack(source_square, occupancy_bitboard[Both])
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_bishop, NoPiece, mailbox[target_square], false, false,
                              false);
        }
    }

    // Rook
    constexpr Piece own_rook = (side == White) ? WhiteRook : BlackRook;
    bitboard = piece_bitboard[own_rook];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetRookAttack(source_square, occupancy_bitboard[Both])
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_rook, NoPiece, mailbox[target_square], false, false,
                              false);
        }
    }

    // Queen
    constexpr Piece own_queen = (side == White) ? WhiteQueen : BlackQueen;
    bitboard = piece_bitboard[own_queen];
    while (bitboard) {
        source_square = bitboard.GetFirstLSBIndex();
        bitboard.RemoveBit(source_square);

        move_bitboard = (MoveGenerator::GetQueenAttack(source_square, occupancy_bitboard[Both])
                         & ~occupancy_bitboard[side]
                         & ~piece_bitboard[opponent_king]);

        capture_move = move_bitboard & occupancy_bitboard[opponent];
        while (capture_move) {
            target_square = capture_move.GetFirstLSBIndex();
            capture_move.RemoveBit(target_square);

            move_list.AddMove(source_square, target_square, own_queen, NoPiece, mailbox[target_square], false, false,
                              false);
        }
    }

    //King
    bitboard = piece_bitboard[own_king];
    source_square = bitboard.GetFirstLSBIndex();

    move_bitboard = (MoveGenerator::GetKingAttack(source_square)
                     & ~occupancy_bitboard[side]
                     & ~piece_bitboard[opponent_king]);

    capture_move = move_bitboard & occupancy_bitboard[opponent];
    while (capture_move) {
        target_square = capture_move.GetFirstLSBIndex();
        capture_move.RemoveBit(target_square);

        move_list.AddMove(source_square, target_square, own_king, NoPiece, mailbox[target_square], false, false, false);
    }
}
