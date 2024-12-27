
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int id;
    char* name;
    char* type;
    char* shape;
    int x;
    int y;
    int width;
    int height;
    int rotation;
    char visible; //bool
    Point* polygon;
    int polygon_count;
    char* properties;
    int isalive;
} Object;

