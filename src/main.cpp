#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

enum class TokenType {
    _return,
    intLit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;
    std::string buffer;
    for (int i = 0; i < str.length(); i++) {
        char c = str[i];
        if (isalpha(c)) {
            buffer.push_back(c);
            i++;
            while (std::isalnum(str[i])) {
                buffer.push_back(str[i]);
                i++;
            }
            i--;
            if (buffer == "return") {
                tokens.push_back({.type = TokenType::_return});
                buffer.clear();
                continue;
            } else {
                std::cerr << "You messed up!" << std::endl;
                exit(2);
            }
        } else if (std::isdigit(c)) {
            buffer.push_back(c);
            i++;
            while (std::isdigit(str[i])) {
                buffer.push_back(str[i]);
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::intLit, .value = buffer});
            buffer.clear();
        } else if (c == ';') {
            tokens.push_back({.type = TokenType::semi});
        } else if (std::isspace(c)) {
            continue;
        } else {
            std::cerr << "You messed up!" << std::endl;
            exit(3);
        }
    }
    return tokens;
}

std::string tokensToASM(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        
        if (token.type == TokenType::_return) {
            if (i+1 < tokens.size() && tokens[i+1].type == TokenType::intLit) {
                if (i+2 < tokens.size() && tokens[i+2].type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens[i+1].value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }

    return output.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect No. of Args." << std::endl;
        std::cerr << "Corect format would look like: ogn <input.ogn>" << std::endl;
        return 1;
    }
    

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream <<input.rdbuf();
        contents = contents_stream.str();
    }

    std::vector<Token> tokens = tokenize(contents);

    {
        std::fstream file("./out.asm", std::ios::out);
        file <<  tokensToASM(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return 0;
} 