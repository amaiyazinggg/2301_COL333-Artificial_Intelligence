#include <iostream>
#include "uciws.hpp"
#include "board.hpp"

#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <iterator>

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

UCIWSServer::UCIWSServer(std::string name, uint32_t port) {
    this->name = name;
    this->port = port;
}

void UCIWSServer::handle_message(ClientConnection conn, const std::string& message) {

    auto toks = split(message, ' ');

    if (toks[0] == "uci") {
        on_uci();
    }
    else if (toks[0] == "isready") {
        on_isready();
    }
    else if (toks[0] == "ucinewgame") {
        on_ucinewgame();
    }
    else if (toks[0] == "position") {
        on_position(toks);
    }
    else if (toks[0] == "go") {
        on_go(toks);
    }
    else if (toks[0] == "stop") {
        on_stop();
    }
    else if (toks[0] == "quit") {
        on_quit();
    }
    else {
        std::cout << "Unsupported message\n";
    }
}

void UCIWSServer::start() {

    //Register our network callbacks, ensuring the logic is run on the main thread's event loop
    server.connect([this](ClientConnection conn)
    {
        this->main_evt_loop.post([conn, this]()
        {
            std::clog << "Connection opened." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
        });
    });

    server.disconnect([this](ClientConnection conn)
    {
        main_evt_loop.post([conn, this]()
        {
            std::clog << "Connection closed." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
        });
    });

    server.message([this](ClientConnection conn, const string& message) {
        this->handle_message(conn, message);
    });
    
    //Start the networking thread
    this->server_thread = std::thread([this]() {
        server.run(port);
    });
    
    //Start the event loop for the main thread
    asio::io_service::work work(main_evt_loop);
    main_evt_loop.run();
}

void UCIWSServer::on_uci() {
    std::cout << "In method on_uci\n";
    server.broadcastMessage("uciok");
}

void UCIWSServer::on_isready() {
    std::cout << "In method on_isready\n";
    server.broadcastMessage("readyok");
}

void UCIWSServer::on_ucinewgame() {
    std::cout << "In method on_ucinewgame\n";
    b = Board();
}

void UCIWSServer::on_position(std::vector<std::string>& toks) {
    std::cout << "In method on_position\n";
    if (toks.size() > 3) {
        b.do_move(str_to_move(toks[toks.size()-1]));
    }
}

void UCIWSServer::on_go(std::vector<std::string>& toks) {
    std::cout << "In method on_go\n";
    // launch a thread to find the best move
    e.search = true;
    this->game_thread = std::thread([this]() {
        e.find_best_move(b);
    });
}

void UCIWSServer::on_stop() {
    std::cout << "In method on_stop\n";
    e.search = false;
    this->game_thread.join();
    U16 move = e.best_move;

    // move checking
    auto legal_moves = b.get_legal_moves();

    assert(legal_moves.size() > 0);
    assert(legal_moves.count(move) > 0);
    b.do_move(move);

    auto str_move = move_to_str(move);
    auto opp_moves = b.get_legal_moves();
    if (b.in_check()) { // opponent is in check because of our move
        if (opp_moves.size() > 0) {
            // check
            str_move += '+';
        }
        else {
            // checkmate
            str_move += '#';
        }
    }
    else if (opp_moves.size() == 0) {
        // stalemate
        str_move += '-';
    }

    server.broadcastMessage("bestmove " + move_to_str(move));
}

void UCIWSServer::on_quit() {
    std::cout << "In method on_quit\n";
    std::exit(0);
}

