//
// Created by Hi on 4/2/2026.
//

#include "../Header/MoveList.h"

void MoveList::PrintList() {
    std::cout << "Move" << std::setw(15) << " Piece" << std::setw(15) << "Promotion" << std::setw(15) <<
            "Capture" << std::setw(15) << "Double Push" << std::setw(15) << "Enpassant" << std::setw(15) << "Castle" <<
            '\n';
    for (int i = 0; i < count; ++i) {
        std::cout << FormatMoveToString(move_list[i]);
    }
    std::cout << "Move count: " << count << '\n';
}

int MoveList::EncodeMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece,
                         int double_push_flag, int enpassant_flag, int castling_flag) {
    uint64_t encoded_move = 0ULL;

    encoded_move |= source_square;
    encoded_move |= (target_square << ENCODE_TARGET_SQUARE_OFFSET);
    encoded_move |= (piece << ENCODE_PIECE_OFFSET);
    encoded_move |= (promoted_piece << ENCODE_PROMOTED_PIECE_OFFSET);
    encoded_move |= (captured_piece << ENCODE_CAPTURED_PIECE_OFFSET);
    encoded_move |= (double_push_flag << ENCODE_DOUBLE_PUSH_FLAG_OFFSET);
    encoded_move |= (enpassant_flag << ENCODE_ENPASSANT_FLAG_OFFSET);
    encoded_move |= (castling_flag << ENCODE_CASTLING_FLAG_OFFSET);

    return encoded_move;
}

Square MoveList::DecodeGetSourceSquare(uint64_t move) {
    return (Square) (move & DECODE_SOURCE_SQUARE_MASK);
}

Square MoveList::DecodeGetTargetSquare(uint64_t move) {
    return (Square) ((move & DECODE_TARGET_SQUARE_MASK) >> ENCODE_TARGET_SQUARE_OFFSET);
}

Piece MoveList::DecodeGetPiece(uint64_t move) {
    return (Piece) ((move & DECODE_PIECE_MASK) >> ENCODE_PIECE_OFFSET);
}

Piece MoveList::DecodeGetPromotedPiece(uint64_t move) {
    return (Piece) ((move & DECODE_PROMOTED_PIECE_MASK) >> ENCODE_PROMOTED_PIECE_OFFSET);
}

Piece MoveList::DecodeGetCapturePiece(uint64_t move) {
    return (Piece) ((move & DECODE_CAPTURED_PIECE_MASK) >> ENCODE_CAPTURED_PIECE_OFFSET);
}

bool MoveList::DecodeGetDoublePushFlag(uint64_t move) {
    return (move & DECODE_DOUBLE_PUSH_FLAG_MASK);
}

bool MoveList::DecodeGetEnpassantFlag(uint64_t move) {
    return (move & DECODE_ENPASSANT_FLAG_MASK);
}

bool MoveList::DecodeGetCastlingFlag(uint64_t move) {
    return (move & DECODE_CASTLING_FLAG_MASK);
}

void MoveList::ClearList() {
    memset(move_list, 0, sizeof(uint64_t) * 256);
    count = 0;
}

void MoveList::AddMove(int move) {
    move_list[count++] = move;
}

void MoveList::AddMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece,
                       int double_push_flag, int enpassant_flag, int castling_flag) {
    AddMove(EncodeMove(source_square, target_square, piece, promoted_piece, captured_piece, double_push_flag,
                       enpassant_flag, castling_flag));
}

std::string MoveList::FormatMoveToString(int encoded_move) {
    std::stringstream ss;

    //Move
    ss << MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(encoded_move)) << MoveGenerator::SquareToString(
        MoveList::DecodeGetTargetSquare(encoded_move));

    //Piece
    ss << std::setw(15) << ChessBoard::PieceToChar(MoveList::DecodeGetPiece(encoded_move));

    //Promotion
    ss << std::setw(15) << ChessBoard::PieceToChar(MoveList::DecodeGetPromotedPiece(encoded_move));

    //Capture
    ss << std::setw(15) << ChessBoard::PieceToChar(MoveList::DecodeGetCapturePiece(encoded_move));

    //Double push
    ss << std::setw(15) << (MoveList::DecodeGetDoublePushFlag(encoded_move) ? "true" : "false");

    //Enpassant
    ss << std::setw(15) << (MoveList::DecodeGetEnpassantFlag(encoded_move) ? "true" : "false");

    //Castling
    ss << std::setw(15) << (MoveList::DecodeGetCastlingFlag(encoded_move) ? "true" : "false");

    ss << '\n';
    return ss.str();
}
