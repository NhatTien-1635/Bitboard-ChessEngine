//
// Created by Hi on 4/2/2026.
//

#include "../Header/MoveList.h"

void MoveList::PrintList() {
    std::ios_base::fmtflags original_flags = std::cout.flags();

    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(10) << "Move"
              << std::setw(10) << "Piece"
              << std::setw(12) << "Promotion"
              << std::setw(10) << "Capture"
              << std::setw(12) << "Double"
              << std::setw(12) << "Enpassant"
              << std::setw(10) << "Castle\n";

    std::cout << std::string(85, '-') << '\n';

    for (int i = 0; i < count; ++i) {
        std::cout << FormatMoveToString(i, move_list[i]);
    }

    std::cout << "\nMove count: " << count << '\n';
    std::cout.flags(original_flags);
}
//
// int MoveList::EncodeMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece,
//                          int double_push_flag, int enpassant_flag, int castling_flag) {
//     uint64_t encoded_move = 0ULL;
//
//     encoded_move |= source_square;
//     encoded_move |= (target_square << ENCODE_TARGET_SQUARE_OFFSET);
//     encoded_move |= (piece << ENCODE_PIECE_OFFSET);
//     encoded_move |= (promoted_piece << ENCODE_PROMOTED_PIECE_OFFSET);
//     encoded_move |= (captured_piece << ENCODE_CAPTURED_PIECE_OFFSET);
//     encoded_move |= (double_push_flag << ENCODE_DOUBLE_PUSH_FLAG_OFFSET);
//     encoded_move |= (enpassant_flag << ENCODE_ENPASSANT_FLAG_OFFSET);
//     encoded_move |= (castling_flag << ENCODE_CASTLING_FLAG_OFFSET);
//
//     return encoded_move;
// }
//
// Square MoveList::DecodeGetSourceSquare(int move) {
//     return (Square) (move & DECODE_SOURCE_SQUARE_MASK);
// }
//
// Square MoveList::DecodeGetTargetSquare(int move) {
//     return (Square) ((move & DECODE_TARGET_SQUARE_MASK) >> ENCODE_TARGET_SQUARE_OFFSET);
// }
//
// Piece MoveList::DecodeGetPiece(int move) {
//     return (Piece) ((move & DECODE_PIECE_MASK) >> ENCODE_PIECE_OFFSET);
// }
//
// Piece MoveList::DecodeGetPromotedPiece(int move) {
//     return (Piece) ((move & DECODE_PROMOTED_PIECE_MASK) >> ENCODE_PROMOTED_PIECE_OFFSET);
// }
//
// Piece MoveList::DecodeGetCapturePiece(int move) {
//     return (Piece) ((move & DECODE_CAPTURED_PIECE_MASK) >> ENCODE_CAPTURED_PIECE_OFFSET);
// }
//
// bool MoveList::DecodeGetDoublePushFlag(int move) {
//     return (move & DECODE_DOUBLE_PUSH_FLAG_MASK);
// }
//
// bool MoveList::DecodeGetEnpassantFlag(int move) {
//     return (move & DECODE_ENPASSANT_FLAG_MASK);
// }
//
// bool MoveList::DecodeGetCastlingFlag(int move) {
//     return (move & DECODE_CASTLING_FLAG_MASK);
// }
//
// void MoveList::ClearList() {
//     memset(move_list, 0, sizeof(int) * 256);
//     count = 0;
// }
//
// void MoveList::AddMove(int move) {
//     move_list[count++] = move;
// }
//
// void MoveList::AddMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece,
//                        int double_push_flag, int enpassant_flag, int castling_flag) {
//     AddMove(EncodeMove(source_square, target_square, piece, promoted_piece, captured_piece, double_push_flag,
//                        enpassant_flag, castling_flag));
// }
//
// int MoveList::GetMove(int index) const {
//     return move_list[index];
// }
//
// int MoveList::GetMoveCount() const {
//     return count;
// }
//
std::string MoveList::FormatMoveToString(int index, int encoded_move) {
    std::stringstream ss;

    // Convert string_view results to a single string to fix the padding gap
    std::string moveStr = std::string(MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(encoded_move))) +
                          std::string(MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(encoded_move)));

    // 1. Index (Tightening the gap between number and move)
    ss << std::left << std::setw(5) << (std::to_string(index) + ".");

    // 2. Move (Pads the whole "e2e4" block)
    ss << std::left << std::setw(10) << moveStr;

    // 3. Piece
    ss << std::left << std::setw(10) << ChessBoard::PieceToChar(MoveList::DecodeGetPiece(encoded_move));

    // 4. Promotion
    ss << std::left << std::setw(12) << ChessBoard::PieceToChar(MoveList::DecodeGetPromotedPiece(encoded_move));

    // 5. Capture
    ss << std::left << std::setw(10) << ChessBoard::PieceToChar(MoveList::DecodeGetCapturePiece(encoded_move));

    // 6. Double push
    ss << std::left << std::setw(12) << (MoveList::DecodeGetDoublePushFlag(encoded_move) ? "true" : "false");

    // 7. Enpassant
    ss << std::left << std::setw(12) << (MoveList::DecodeGetEnpassantFlag(encoded_move) ? "true" : "false");

    // 8. Castling
    ss << std::left << std::setw(10) << (MoveList::DecodeGetCastlingFlag(encoded_move) ? "true" : "false");

    ss << '\n';
    return ss.str();
}
