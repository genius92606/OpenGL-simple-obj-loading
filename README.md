繪圖技術設計與應用HW1-筆記
============
作業環境
===
Windows10, visual studio 2015
* GLFW
允許使用者創建OpenGL上下文，定義窗口參數以及處理輸入
* GML 
OpenGL Mathematics，幫助數學運算的函示庫
* GLEW
OpenGL Extension Wrangler provides，能自動識別使用者的平台所支持的全部OpenGL高級況展函數。

所以我們把以上include進自己的專案中

1. include檔案夾包含各種.h檔
2. lib檔案夾包含各種lib檔案，讓動態連結程式庫(dll)去取用

light.frag & light.vert 用來setup shader

方法說明
====

進入build
開啟ConsoleApplication.sln按執行或
進入Release開啟ConsoleApplication.exe

程式如何操作
==

1. 一開始GLFW 初始化
2. set the hint before glfwCreateWindow
3. 然後 create window
4. 設context
5. 確認keycallback  在這裡我們用到"ESC"
6. load shader
7. add_obj by "tiny_obj_loader"
    * 產生VBO、VAO 然後bindbuffer
8. 開始render~~~
    * set the camera matrix
    * 可以參考以下網站~
     > https://learnopengl.com/Getting-started/Camera
    * bind VAO
    * set translate and rotate matrix
    * draw objects~~~
    * unbind VAO
9. 關視窗


