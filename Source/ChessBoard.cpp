//
// Created by Hi on 3/23/2026.
//

#include "../Header/ChessBoard.h"
#include "../Header/MoveList.h"

const uint8_t ChessBoard::castling_right_mask[64] = {
#include "../Data/CastlingRightMatrix.dat"
};

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

bool ChessBoard::MakeQuietMove(int encoded_move) {
    //Reserve board state
    history[game_ply++] = board_state;

    Square source_square = MoveList::DecodeGetSourceSquare(encoded_move);
    Square target_square = MoveList::DecodeGetTargetSquare(encoded_move);
    Piece piece = MoveList::DecodeGetPiece(encoded_move);

    //Quiet move
    piece_bitboard[piece].InsertBit(target_square);
    occupancy_bitboard[board_state.side_to_move].InsertBit(target_square);
    mailbox[target_square] = piece;

    piece_bitboard[piece].RemoveBit(source_square);
    occupancy_bitboard[board_state.side_to_move].RemoveBit(source_square);
    mailbox[source_square] = NoPiece;

    //Capture
    Piece capture_piece = MoveList::DecodeGetCapturePiece(encoded_move);
    bool en_passant = MoveList::DecodeGetEnpassantFlag(encoded_move);
    if (capture_piece != NoPiece && !en_passant) {
        piece_bitboard[capture_piece].RemoveBit(target_square);
        occupancy_bitboard[opponent_side[board_state.side_to_move]].RemoveBit(target_square);
    }

    //Promotion
    Piece promoted_piece = MoveList::DecodeGetPromotedPiece(encoded_move);
    if (promoted_piece != NoPiece) {
        piece_bitboard[piece].RemoveBit(target_square);
        piece_bitboard[promoted_piece].InsertBit(target_square);

        mailbox[target_square] = promoted_piece;
    }

    //Double
    bool double_push = MoveList::DecodeGetDoublePushFlag(encoded_move);
    if (double_push) {
        if (board_state.side_to_move == White) {
            board_state.en_passant = target_square + 8;
        } else {
            board_state.en_passant = target_square - 8;
        }
    }

    //En passant
    if (en_passant) {
        if (board_state.side_to_move == White) {
            piece_bitboard[capture_piece].RemoveBit(target_square + 8);
            occupancy_bitboard[opponent_side[board_state.side_to_move]].RemoveBit(target_square + 8);
            mailbox[target_square + 8] = NoPiece;
        } else {
            piece_bitboard[capture_piece].RemoveBit(target_square - 8);
            occupancy_bitboard[opponent_side[board_state.side_to_move]].RemoveBit(target_square - 8);
            mailbox[target_square - 8] = NoPiece;
        }
    }

    //Castle
    bool castling = MoveList::DecodeGetCastlingFlag(encoded_move);
    if (castling) {
        if (board_state.side_to_move == White) {
            // Castling queen side
            if (target_square == c1) {
                piece_bitboard[WhiteRook].RemoveBit(a1);
                piece_bitboard[WhiteRook].InsertBit(d1);

                occupancy_bitboard[board_state.side_to_move].RemoveBit(a1);
                occupancy_bitboard[board_state.side_to_move].InsertBit(d1);

                mailbox[a1] = NoPiece;
                mailbox[d1] = WhiteRook;
            }
            // Castling king side
            else if (target_square == g1) {
                piece_bitboard[WhiteRook].RemoveBit(h1);
                piece_bitboard[WhiteRook].InsertBit(f1);

                occupancy_bitboard[board_state.side_to_move].RemoveBit(h1);
                occupancy_bitboard[board_state.side_to_move].InsertBit(f1);

                mailbox[h1] = NoPiece;
                mailbox[f1] = WhiteRook;
            }
        } else {
            // Castling queen side
            if (target_square == c8) {
                piece_bitboard[BlackRook].RemoveBit(a8);
                piece_bitboard[BlackRook].InsertBit(d8);

                occupancy_bitboard[board_state.side_to_move].RemoveBit(a8);
                occupancy_bitboard[board_state.side_to_move].InsertBit(d8);

                mailbox[a8] = NoPiece;
                mailbox[d8] = BlackRook;
            }
            // Castling king side
            else if (target_square == g8) {
                piece_bitboard[BlackRook].RemoveBit(h8);
                piece_bitboard[BlackRook].InsertBit(f8);

                occupancy_bitboard[board_state.side_to_move].RemoveBit(h8);
                occupancy_bitboard[board_state.side_to_move].InsertBit(f8);

                mailbox[h8] = NoPiece;
                mailbox[f8] = BlackRook;
            }
        }
    }

    //Update occupancy bitboard
    occupancy_bitboard[Both] = (occupancy_bitboard[White] | occupancy_bitboard[Black]);

    //Update side to move
    board_state.side_to_move = opponent_side[board_state.side_to_move];

    //Update square to move if not double push
    if (!double_push) {
        board_state.en_passant = NoSquare;
    }

    //Update castling
    board_state.castling_right &= (castling_right_mask[source_square] & castling_right_mask[target_square]);

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
    int king_square = piece_bitboard[opponent_side[board_state.side_to_move] * 6 + WhiteKing].GetFirstLSBIndex();
    Side opponent = (Side)board_state.side_to_move;
    if (IsSquaredAttacked(king_square, opponent)) {
        UnmakeMove(encoded_move);
        return false;
    }

    return true;
}

bool ChessBoard::MakeCaptureMove(int encoded_move) {
    if (MoveList::DecodeGetCapturePiece(encoded_move) != NoPiece) {
        return MakeQuietMove(encoded_move);
    }
    //Move is not capture move
    else {
        return false;
    }
}

void ChessBoard::UnmakeMove(int encoded_move) {
    //Restore game state
    board_state = history[--game_ply];

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
    if (captured_piece != NoPiece && !en_passant) {
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
        //Side to move haven't updated yet
        if (board_state.side_to_move == White) {
            piece_bitboard[captured_piece].InsertBit(target_square + 8);
            occupancy_bitboard[opponent_side[board_state.side_to_move]].InsertBit(target_square + 8);
            mailbox[target_square + 8] = captured_piece;
        } else {
            piece_bitboard[captured_piece].InsertBit(target_square - 8);
            occupancy_bitboard[opponent_side[board_state.side_to_move]].InsertBit(target_square - 8);
            mailbox[target_square - 8] = captured_piece;
        }
    }

    //Castling
    bool castling = MoveList::DecodeGetCastlingFlag(encoded_move);
    if (castling) {
        if (board_state.side_to_move == White) {
            // Castling queen side
            if (target_square == c1) {
                piece_bitboard[WhiteRook].RemoveBit(d1);
                occupancy_bitboard[White].RemoveBit(d1);
                piece_bitboard[WhiteRook].InsertBit(a1);
                occupancy_bitboard[White].InsertBit(a1);

                mailbox[a1] = WhiteRook;
                mailbox[d1] = NoPiece;
            }
            // Castling king side
            else if (target_square == g1) {
                piece_bitboard[WhiteRook].RemoveBit(f1);
                occupancy_bitboard[White].RemoveBit(f1);
                piece_bitboard[WhiteRook].InsertBit(h1);
                occupancy_bitboard[White].InsertBit(h1);

                mailbox[h1] = WhiteRook;
                mailbox[f1] = NoPiece;
            }
        } else {
            // Castling queen side
            if (target_square == c8) {
                piece_bitboard[BlackRook].RemoveBit(d8);
                occupancy_bitboard[Black].RemoveBit(d8);
                piece_bitboard[BlackRook].InsertBit(a8);
                occupancy_bitboard[Black].InsertBit(a8);

                mailbox[a8] = BlackRook;
                mailbox[d8] = NoPiece;
            }
            // Castling king side
            else if (target_square == g8) {
                piece_bitboard[BlackRook].RemoveBit(f8);
                occupancy_bitboard[Black].RemoveBit(f8);
                piece_bitboard[BlackRook].InsertBit(h8);
                occupancy_bitboard[Black].InsertBit(h8);


                mailbox[h8] = BlackRook;
                mailbox[f8] = NoPiece;
            }
        }
    }

    //Restore occupancy bitboard
    occupancy_bitboard[Both] = (occupancy_bitboard[White] | occupancy_bitboard[Black]);


}

//Check if the square is attacked by the attacker Side
bool ChessBoard::IsSquaredAttacked(uint8_t square, uint8_t attacker) const {
    //Calculate offset
    int offset = attacker * 6;

    //Grap the correct piece
    Bitmap pawn_bits = piece_bitboard[offset + WhitePawn];
    Bitmap knight_bits = piece_bitboard[offset + WhiteKnight];
    Bitmap bishop_bits = piece_bitboard[offset + WhiteBishop] | piece_bitboard[offset + WhiteQueen];
    Bitmap rook_bits = piece_bitboard[offset + WhiteRook] | piece_bitboard[offset + WhiteQueen];
    Bitmap king_bits = piece_bitboard[offset + WhiteKing];

    //Checking pawn attack
    Side victim_side = opponent_side[attacker];
    if (MoveGenerator::GetPawnAttack(square, victim_side) & pawn_bits) return true;

    //Checking knight attack
    if (MoveGenerator::GetKnightAttack(square) & knight_bits) return true;

    //Checking sliding attacks
    if (MoveGenerator::GetBishopAttack(square, occupancy_bitboard[Both]) & bishop_bits) return true;
    if (MoveGenerator::GetRookAttack(square, occupancy_bitboard[Both]) & rook_bits) return true;

    //Checking king attack
    if (MoveGenerator::GetKingAttack(square) & king_bits) return true;

    return false;
}

ChessBoard::ChessBoard(const std::string_view &FEN) {
    ParsePositionFromFEN(FEN);
}

Bitmap ChessBoard::GetPieceBitmap(int piece) const {
    return piece_bitboard[piece];
}

Piece ChessBoard::At(int index) const {
    return (Piece) mailbox[index];
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
    int source_square = 0;
    int target_square = 0;

    Bitmap bitboard = 0;
    Bitmap attack = 0;

    int start = board_state.side_to_move == White ? 0 : 6;
    int end = board_state.side_to_move == White ? 6 : 12;

    for (int piece = start; piece < end; ++piece) {
        bitboard = piece_bitboard[piece];

        switch (piece % 6) {
            //Pawn
            case 0:
                while (bitboard) {
                    source_square = bitboard.GetFirstLSBIndex();
                    bitboard.RemoveBit(source_square);

                    //White pawn move
                    if (board_state.side_to_move == White) {
                        //Quiet pawn move
                        target_square = source_square - 8;
                        if (mailbox[target_square] == NoPiece) {
                            //Promotion
                            if (target_square >= a8 && target_square <= h8) {
                                for (int i = 1; i < 5; ++i) {
                                    move_list.AddMove(source_square, target_square, WhitePawn, i,
                                                      mailbox[target_square], false, false,
                                                      false);
                                }
                            } else {
                                move_list.AddMove(source_square, target_square, WhitePawn, NoPiece,
                                                  mailbox[target_square], false, false,
                                                  false);
                            }

                            //Double pawn move
                            if (source_square >= a2 && source_square <= h2) {
                                target_square = source_square - 16;
                                if (mailbox[target_square] == NoPiece)
                                    move_list.AddMove(source_square, target_square, WhitePawn, NoPiece,
                                                      mailbox[target_square], true,
                                                      false,
                                                      false);
                            }
                        }

                        //Attack
                        attack = MoveGenerator::GetPawnAttack(source_square, (Side)board_state.side_to_move);
                        if (board_state.en_passant != NoSquare && (attack & Bitmap(1ULL << board_state.en_passant))) {
                            move_list.AddMove(source_square, board_state.en_passant, WhitePawn, NoPiece,
                                              BlackPawn, false, true, false);
                        }
                        attack &= occupancy_bitboard[Black];

                        while (attack) {
                            target_square = attack.GetFirstLSBIndex();
                            attack.RemoveBit(target_square);

                            //Promotion
                            if (target_square >= a8 && target_square <= h8) {
                                for (int i = 1; i < 5; ++i) {
                                    move_list.AddMove(source_square, target_square, WhitePawn, i,
                                                      mailbox[target_square], false, false,
                                                      false);
                                }
                            } else {
                                move_list.AddMove(source_square, target_square, WhitePawn, NoPiece,
                                                  mailbox[target_square], false, false,
                                                  false);
                            }
                        }

                        //Black pawn move
                    } else {
                        //Quiet pawn move
                        target_square = source_square + 8;
                        if (mailbox[target_square] == NoPiece) {
                            //Promotion
                            if (target_square >= a1 && target_square <= h1) {
                                for (int i = 7; i < 11; ++i) {
                                    move_list.AddMove(source_square, target_square, BlackPawn, i,
                                                      mailbox[target_square], false, false,
                                                      false);
                                }
                            } else {
                                move_list.AddMove(source_square, target_square, BlackPawn, NoPiece,
                                                  mailbox[target_square], false,
                                                  false,
                                                  false);
                            }

                            //Double pawn move
                            if (source_square >= a7 && source_square <= h7) {
                                target_square = source_square + 16;
                                if (mailbox[target_square] == NoPiece)
                                    move_list.AddMove(source_square, target_square, BlackPawn, NoPiece,
                                                      mailbox[target_square], true,
                                                      false,
                                                      false);
                            }
                        }

                        //Attack
                        attack = MoveGenerator::GetPawnAttack(source_square, (Side)board_state.side_to_move);
                        if (board_state.en_passant != NoSquare && (attack & Bitmap(1ULL << board_state.en_passant))) {
                            move_list.AddMove(source_square, board_state.en_passant, BlackPawn, NoPiece,
                                              WhitePawn, false, true, false);
                        }
                        attack &= occupancy_bitboard[White];
                        while (attack) {
                            target_square = attack.GetFirstLSBIndex();
                            attack.RemoveBit(target_square);

                            if (target_square >= a1 && target_square <= h1) {
                                for (int i = 7; i < 11; ++i) {
                                    move_list.AddMove(source_square, target_square, BlackPawn, i,
                                                      mailbox[target_square], false, false,
                                                      false);
                                }
                            } else {
                                move_list.AddMove(source_square, target_square, BlackPawn, NoPiece,
                                                  mailbox[target_square], false, false,
                                                  false);
                            }
                        }
                    }
                }
                break;

            //Knight
            case 1:
                while (bitboard) {
                    source_square = bitboard.GetFirstLSBIndex();
                    bitboard.RemoveBit(source_square);

                    attack = MoveGenerator::GetKnightAttack(source_square);
                    attack &= ~(occupancy_bitboard[board_state.side_to_move]);

                    while (attack) {
                        target_square = attack.GetFirstLSBIndex();
                        attack.RemoveBit(target_square);

                        move_list.AddMove(source_square, target_square,
                                          mailbox[source_square], NoPiece, mailbox[target_square], false,
                                          false, false);
                    }
                }
                break;

            //Bishop
            case 2:
                while (bitboard) {
                    source_square = bitboard.GetFirstLSBIndex();
                    bitboard.RemoveBit(source_square);

                    attack = MoveGenerator::GetBishopAttack(source_square, occupancy_bitboard[Both]);
                    attack &= ~(occupancy_bitboard[board_state.side_to_move]);

                    while (attack) {
                        target_square = attack.GetFirstLSBIndex();
                        attack.RemoveBit(target_square);

                        move_list.AddMove(source_square, target_square,
                                          mailbox[source_square], NoPiece, mailbox[target_square], false,
                                          false, false);
                    }
                }
                break;

            //Rook
            case 3:
                while (bitboard) {
                    source_square = bitboard.GetFirstLSBIndex();
                    bitboard.RemoveBit(source_square);

                    attack = MoveGenerator::GetRookAttack(source_square, occupancy_bitboard[Both]);
                    attack &= ~(occupancy_bitboard[board_state.side_to_move]);

                    while (attack) {
                        target_square = attack.GetFirstLSBIndex();
                        attack.RemoveBit(target_square);

                        move_list.AddMove(source_square, target_square,
                                          mailbox[source_square], NoPiece, mailbox[target_square], false,
                                          false, false);
                    }
                }
                break;

            //Queen
            case 4:
                while (bitboard) {
                    source_square = bitboard.GetFirstLSBIndex();
                    bitboard.RemoveBit(source_square);

                    attack = MoveGenerator::GetQueenAttack(source_square, occupancy_bitboard[Both]);
                    attack &= ~(occupancy_bitboard[board_state.side_to_move]);

                    while (attack) {
                        target_square = attack.GetFirstLSBIndex();
                        attack.RemoveBit(target_square);

                        move_list.AddMove(source_square, target_square,
                                          mailbox[source_square], NoPiece, mailbox[target_square], false,
                                          false, false);
                    }
                }
                break;

            //King
            case 5:
                source_square = bitboard.GetFirstLSBIndex();

                attack = MoveGenerator::GetKingAttack(source_square);
                attack &= ~(occupancy_bitboard[board_state.side_to_move]);

                while (attack) {
                    target_square = attack.GetFirstLSBIndex();
                    attack.RemoveBit(target_square);

                    move_list.AddMove(source_square, target_square,
                                      mailbox[source_square], NoPiece, mailbox[target_square], false,
                                      false, false);
                }

                //White castling
                if (board_state.side_to_move == White) {
                    if (IsSquaredAttacked(source_square, Black)) {
                        break;
                    }

                    if (board_state.castling_right & WhiteToQueen) {
                        bool no_attack = (!IsSquaredAttacked(c1, Black) && !IsSquaredAttacked(d1, Black));
                        bool no_occupancy = ((mailbox[c8] == NoPiece) && (mailbox[c1] == NoPiece) && (mailbox[d1] == NoPiece));
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
                        break;
                    }

                    if (board_state.castling_right & BlackToQueen) {
                        bool no_attack = (!IsSquaredAttacked(c8, Black) && !IsSquaredAttacked(d8, Black));
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
                break;

            default:
                break;
        }
    }
}

ChessBoard::ChessBoard() {
    ParsePositionFromFEN(FEN_STARTING_POSITION);
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
