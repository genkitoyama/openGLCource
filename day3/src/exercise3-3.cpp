#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLFW_INCLUDE_GLU
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// ディレクトリの設定ファイル
#include "common.h"

static int WIN_WIDTH   = 500;                       // ウィンドウの幅
static int WIN_HEIGHT  = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const double PI = 4.0 * std::atan(1.0);

// シェーダファイル
static const std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render2.vert";
static const std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render2.frag";

// 頂点オブジェクト
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec2 &texcoord_)
    : position(position_)
    , texcoord(texcoord_){
    }
    
    glm::vec3 position;
    glm::vec2 texcoord;
};

static const glm::vec3 positions[8] = {
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3( 1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3( 1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f,  1.0f,  1.0f)
};

static const glm::vec3 colors[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f),  // 赤
    glm::vec3(0.0f, 1.0f, 0.0f),  // 緑
    glm::vec3(0.0f, 0.0f, 1.0f),  // 青
    glm::vec3(1.0f, 1.0f, 0.0f),  // イエロー
    glm::vec3(0.0f, 1.0f, 1.0f),  // シアン
    glm::vec3(1.0f, 0.0f, 1.0f),  // マゼンタ
};

static const unsigned int faces[12][3] = {
    { 1, 6, 7 }, { 1, 7, 4 },
    { 2, 5, 7 }, { 2, 7, 4 },
    { 3, 5, 7 }, { 3, 7, 6 },
    { 0, 1, 4 }, { 0, 4, 2 },
    { 0, 1, 6 }, { 0, 6, 3 },
    { 0, 2, 5 }, { 0, 5, 3 }
};

static const glm::vec2 texcoords[14] = {      //uv座標
    glm::vec2(0.25f,  0.0f),
    glm::vec2(0.5f,  0.0f),
    glm::vec2(0.0f,  0.33f),
    glm::vec2(0.25f,  0.33f),
    glm::vec2(0.5f,  0.33f),
    glm::vec2(0.75f,  0.33f),
    glm::vec2(1.0f,  0.33f),
    glm::vec2(0.0f,  0.67f),
    glm::vec2(0.25f,  0.67f),
    glm::vec2(0.5f,  0.67f),
    glm::vec2(0.75f,  0.67f),
    glm::vec2(1.0f,  0.67f),
    glm::vec2(0.25f,  1.0f),
    glm::vec2(0.5f,  1.0f),
};

static const unsigned int texIndices[12][3] = {
    { 0, 1, 4 }, { 0, 4, 3 },
    { 2, 3, 8 }, { 2, 8, 7 },
    { 3, 4, 9 }, { 3, 9, 8 },
    { 5, 6, 11 }, { 5, 11, 10 },
    { 4, 5, 10 }, { 4, 10, 9 },
    { 8, 9, 13 }, { 8, 13, 12 },
    
};

// バッファを参照する番号
GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// 頂点番号配列の大きさ
static size_t indexBufferSize = 48;

// シェーダを参照する番号
GLuint programId;

// 立方体の回転角度
static float theta = 0.0f;

// シェーディングのための情報
static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "dice.png";
GLuint textureId;

// VAOの初期化
void initVAO() {
    // Vertex配列の作成
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int idx = 0;
    for (int face = 0; face < 6; face++) {
        for (int i = 0; i < 3; i++) {
            vertices.push_back(Vertex(positions[faces[face * 2 + 0][i]], texcoords[texIndices[face * 2 + 0][i]]));
            indices.push_back(idx++);
        }
        
        for (int i = 0; i < 3; i++) {
            vertices.push_back(Vertex(positions[faces[face * 2 + 1][i]], texcoords[texIndices[face * 2 + 1][i]]));
            indices.push_back(idx++);
        }
    }
    
    // VAOの作成
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    
    // 頂点バッファの作成
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    // 頂点バッファの有効化
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    
    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
    
    // VAOをOFFにしておく
    glBindVertexArray(0);
}

GLuint compileShader(const std::string &filename, GLuint type) {
    // シェーダの作成
    GLuint shaderId = glCreateShader(type);
    
    // ファイルの読み込み
    std::ifstream reader;
    size_t codeSize;
    std::string code;
    
    // ファイルを開く
    reader.open(filename.c_str(), std::ios::in);
    if (!reader.is_open()) {
        // ファイルを開けなかったらエラーを出して終了
        fprintf(stderr, "Failed to load a shader: %s\n", VERT_SHADER_FILE.c_str());
        exit(1);
    }
    
    // ファイルをすべて読んで変数に格納 (やや難)
    reader.seekg(0, std::ios::end);             // ファイル読み取り位置を終端に移動
    codeSize = reader.tellg();                  // 現在の箇所(=終端)の位置がファイルサイズ
    code.resize(codeSize);                      // コードを格納する変数の大きさを設定
    reader.seekg(0);                            // ファイルの読み取り位置を先頭に移動
    reader.read(&code[0], codeSize);            // 先頭からファイルサイズ分を読んでコードの変数に格納
    
    // ファイルを閉じる
    reader.close();
    
    // コードのコンパイル
    const char *codeChars = code.c_str();
    glShaderSource(shaderId, 1, &codeChars, NULL);
    glCompileShader(shaderId);
    
    // コンパイルの成否を判定する
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        // コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
        fprintf(stderr, "Failed to compile a shader!\n");
        
        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);
            
            // エラーメッセージとソースコードの出力
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
            fprintf(stderr, "%s\n", code.c_str());
        }
        exit(1);
    }
    
    return shaderId;
}

GLuint buildShaderProgram(const std::string &vShaderFile, const std::string &fShaderFile) {
    // シェーダの作成
    GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
    GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);
    
    // シェーダプログラムのリンク
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    
    // リンクの成否を判定する
    GLint linkState;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
    if (linkState == GL_FALSE) {
        // リンクに失敗したらエラーメッセージを表示して終了
        fprintf(stderr, "Failed to link shaders!\n");
        
        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);
            
            // エラーメッセージを出力する
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
        }
        exit(1);
    }
    
    // シェーダを無効化した後にIDを返す
    glUseProgram(0);
    return programId;
}

// シェーダの初期化
void initShaders() {
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

void initTexture() {
    // テクスチャの設定
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
        exit(1);
    }
    
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(bytes);
}

// OpenGLの初期化関数
void initializeGL() {
    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);
    
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // VAOの初期化
    initVAO();
    
    // シェーダの用意
    initShaders();
    
    // テクスチャの初期化
    initTexture();
}

// OpenGLの描画関数
void paintGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 座標の変換
    glm::mat4 projMat = glm::perspective(45.0f,
                                         (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);
    
    glm::mat4 viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
                                    glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                    glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向
    
    float rotateX = cos(glfwGetTime())*0.5f + 0.5f;
    float rotateY = sin(glfwGetTime())*0.5f + 0.5f;
    
    glm::mat4 modelMat = glm::rotate(glm::radians(theta), glm::vec3(rotateX, rotateY, 0.0f));
    
    glm::mat4 mvMat = viewMat * modelMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat;
    glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
    glm::mat4 lightMat = viewMat;
    
    // VAOの有効化
    glBindVertexArray(vaoId);
    
    // シェーダの有効化
    glUseProgram(programId);
    
    // Uniform変数の転送
    GLuint uid;
    uid = glGetUniformLocation(programId, "u_mvMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvMat));
    uid = glGetUniformLocation(programId, "u_mvpMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
    uid = glGetUniformLocation(programId, "u_normMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(normMat));
    uid = glGetUniformLocation(programId, "u_lightMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(lightMat));
    uid = glGetUniformLocation(programId, "u_lightPos");
    glUniform3fv(uid, 1, glm::value_ptr(lightPos));
    
    // テクスチャの有効化とシェーダへの転送
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    uid = glGetUniformLocation(programId, "u_texture");
    glUniform1i(uid, 0);
    
    // 三角形の描画
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);
    
    // VAOの無効化
    glBindVertexArray(0);
    
    // シェーダの無効化
    glUseProgram(0);
    
    // テクスチャの無効化
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void resizeGL(GLFWwindow *window, int width, int height) {
    // ユーザ管理のウィンドウサイズを変更
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    
    // GLFW管理のウィンドウサイズを変更
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
    
    // 実際のウィンドウサイズ (ピクセル数) を取得
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);
    
    // ビューポート変換の更新
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

// アニメーションのためのアップデート
void animate() {
    theta += 3.0f;  // 3度だけ回転
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }
    
    // OpenGLのバージョン設定 (Macの場合には必ず必要)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Windowの作成
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
        return 1;
    }
    
    // OpenGLの描画対象にWindowを追加
    glfwMakeContextCurrent(window);
    
    // GLEWを初期化する (glfwMakeContextCurrentの後でないといけない)
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization failed!\n");
        return 1;
    }
    
    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);
    
    // OpenGLを初期化
    initializeGL();
    
    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paintGL();
        
        // アニメーション
        animate();
        
        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
