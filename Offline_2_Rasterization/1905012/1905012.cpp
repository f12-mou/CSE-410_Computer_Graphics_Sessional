#include<bits/stdc++.h>
#include <fstream>
using namespace std;
#include "bitmap_image.hpp"

#define pi (2*acos(0.0))
#define endl '\n'
static unsigned long long int g_seed = 1;
inline int random()
{
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

class Point
{
public:
    double x,y,z,n;
    Point()
    {
        x=0;
        y=0;
        z=0;
        n=1.0;
    }
    Point(double x, double y, double z, double n) : x(x), y(y), z(z), n(n) {}
    Point(double x2, double y2, double z2)
    {
        x=x2;
        y=y2;
        z=z2;
    }

    Point(const Point &p) : x(p.x), y(p.y), z(p.z), n(p.n) {}
    void scaleDown()
    {
        x /= n;
        y /= n;
        z /= n;
        n = 1.0;
    }

    Point operator +(Point b)
    {
        return Point(x+b.x,y+b.y, z+b.z);
    }
    Point operator -(Point b)
    {
        return Point(x-b.x,y-b.y, z-b.z);
    }
    Point operator *(double b)
    {
        return Point(x*b,y*b, z*b);
    }
    Point operator /(double b)
    {
        return Point(x/b,y/b, z/b);
    }
    Point operator *(Point b)
    {
        return Point(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x);
    }
};

double dot(Point a, Point b)
{
    return a.x * b.x + a.y * b.y + a.z*b.z;
}
double length(Point a)
{
    return sqrt(dot(a, a));
}
Point normalize(Point a)
{
    return a/length(a);
}

Point unit_i(1,0,0);
Point unit_j(0,1,0);
Point unit_k(0,0,1);
double  eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ, fovY, aspectRatio, near, far;


stack<vector<double>>globalStack, localStack;
stack<int>globalStack_indicator;
vector<Point>stage1_outputs, stage2_outputs, stage3_outputs;

vector<double>rotation_vec{0,0,0,0,
           0,0,0,0,
           0,0,0,0,
           0,0,0,1};
vector<double>translation_vec{1,0,0,0,
           0,1,0,0,
           0,0,1,0,
           0,0,0,1};
vector<double>scaling_vec{1,0,0,0,
           0,1,0,0,
           0,0,1,0,
           0,0,0,1};
vector<double>final_vec{0,0,0,0,
           0,0,0,0,
           0,0,0,0,
           0,0,0,0};
vector<double>identity_vec{1,0,0,0,
           0,1,0,0,
           0,0,1,0,
           0,0,0,1};


void calcRotationMatrix(Point c1, Point c2, Point c3)
{
    vector<double>now_rotation_vec = rotation_vec;
    // x ordinates
    now_rotation_vec[0]=c1.x;
    now_rotation_vec[1]=c2.x;
    now_rotation_vec[2]=c3.x;

    // y ordinates
    now_rotation_vec[4]=c1.y;
    now_rotation_vec[5]=c2.y;
    now_rotation_vec[6]=c3.y;
    // z ordinates
    now_rotation_vec[8]=c1.z;
    now_rotation_vec[9]=c2.z;
    now_rotation_vec[10]=c3.z;

    localStack.push(now_rotation_vec);
}
void calcTranslateMatrix(double sx, double sy, double sz)
{
    vector<double>now_translation_vec = translation_vec;
    now_translation_vec[3]=sx;
    now_translation_vec[7]=sy;
    now_translation_vec[11]=sz;
    localStack.push(now_translation_vec);
}
void calcScalingMatrix(double tx, double ty, double tz)
{
    vector<double>now_scaling_vec = scaling_vec;
    now_scaling_vec[0]=tx;
    now_scaling_vec[5]=ty;
    now_scaling_vec[10]=tz;
    localStack.push(now_scaling_vec);
}
vector<double> matrixMultiplication(vector<double>vec1, vector<double>vec2)
{
    double arr1[5][5], arr2[5][5], ans[5][5]= {0,0};
    int start=0;
    for(int i=1; i<=4; i++)
    {
        for(int j=1; j<=4; j++)
        {
            arr1[i][j]=vec1[start++];
        }
    }
    start=0;
    for(int i=1; i<=4; i++)
    {
        for(int j=1; j<=4; j++)
        {
            arr2[i][j]=vec2[start++];
        }
    }
    for(int i = 1; i <= 4; ++i)
        for(int j = 1; j <= 4; ++j)
            for(int k = 1; k <= 4; ++k)
            {
                ans[i][j] += arr1[i][k] * arr2[k][j];
            }
    vector<double>finalAns;
    for(int i=1; i<=4; i++)
    {
        for(int j=1; j<=4; j++)
        {
            finalAns.push_back(ans[i][j]);
        }
    }
    return finalAns;


}
void calcFinalMatrix()
{
    stack<vector<double>>nowLocalStack = localStack;
    vector<double>ans = nowLocalStack.top();
    nowLocalStack.pop();
    while(nowLocalStack.empty()==false)
    {
        vector<double>leftVec = nowLocalStack.top();
        nowLocalStack.pop();
        ans = matrixMultiplication(leftVec, ans);
    }
    final_vec = ans;
}


Point matrixPointMultiplication(Point p, double nnn=1.0)
{
    double arr1[5][5], arr2[5][5], ans[5][5]= {0.0,0.0};
    int start=0;
    for(int i=1; i<=4; i++)
    {
        for(int j=1; j<=4; j++)
        {
            arr1[i][j]=final_vec[start++];
        }
    }
    arr2[1][1]=p.x;
    arr2[2][1]=p.y;
    arr2[3][1]=p.z;
    arr2[4][1]=nnn;
    //arr2[4][1]=p.n;
    for(int i = 1; i <= 4; ++i)
        for(int j = 1; j <= 1; ++j)
            for(int k = 1; k <= 4; ++k)
            {
                ans[i][j] += arr1[i][k] * arr2[k][j];
            }
    Point ansPoint(ans[1][1], ans[2][1], ans[3][1], ans[4][1]);
    //ansPoint.scaleDown();
    //cout<<"returning "<<ansPoint.x<<" "<<ansPoint.y<<" "<<ansPoint.z<<endl;
    return ansPoint;
}


Point RodriguesFormula(Point x, Point a, double theta)
{
    theta = (pi/180.0)*theta;
    Point ans = (x*cos(theta))+a*((1-(cos(theta)))*(dot(a,x)))+(a*x)*(sin(theta));
    return ans;
}

void stage2(Point eye, Point look, Point up)
{
    Point l = look-eye;
    l = normalize(l);
    Point r = l * up;
    r = normalize(r);
    Point u = r * l;
    vector<double>translate_vec = identity_vec;
    translate_vec[3]=-eyeX;
    translate_vec[7]=-eyeY;
    translate_vec[11]=-eyeZ;

    vector<double>rotate_vec = identity_vec;
    rotate_vec[0] = r.x;
    rotate_vec[1] = r.y;
    rotate_vec[2] = r.z;

    rotate_vec[4] = u.x;
    rotate_vec[5] = u.y;
    rotate_vec[6] = u.z;

    rotate_vec[8] = -l.x;
    rotate_vec[9] = -l.y;
    rotate_vec[10] = -l.z;

    vector<double>final_vec_stage_2 = matrixMultiplication(rotate_vec, translate_vec);
    final_vec = final_vec_stage_2;
    ofstream fileout("stage2.txt");
    for(int i=0; i<stage1_outputs.size(); i++)
    {
        if(i%3==0)
        {
            fileout<<endl;
        }
        Point here=stage1_outputs[i];
        Point ansPoint = matrixPointMultiplication(here);

        stage2_outputs.push_back(ansPoint);
        fileout<<setprecision(7)<<fixed<<ansPoint.x<<" "<<ansPoint.y<<" "<<ansPoint.z<<endl;
    }
    fileout.close();
}

void stage3()
{
    double fovX = (fovY) * aspectRatio;
    double t = near * tan(fovY*pi/360.0);
    double r = near * tan(fovX*pi/360.0);
    vector<double>projection_vec = identity_vec;
    projection_vec[0]=near/r;
    projection_vec[5]=near/t;
    projection_vec[10]=-(far+near)/(far-near);
    projection_vec[11]=-(2.0*far*near)/(far-near);
    projection_vec[14]=-1.0;
    projection_vec[15]=0;
    final_vec = projection_vec;

    int start=0;

    ofstream fileout("stage3.txt");
    for(int i=0; i<stage2_outputs.size(); i++)
    {
        if(i%3==0)
        {
            fileout<<endl;
        }
        Point here=stage2_outputs[i];
        Point ansPoint = matrixPointMultiplication(here, 1.0);
        ansPoint.scaleDown();
        stage3_outputs.push_back(ansPoint);
        fileout<< setprecision(7)<<fixed<<ansPoint.x<<" "<<ansPoint.y<<" "<<ansPoint.z<<endl;
    }
    fileout.close();
}
void printStage1Output()
{
    ofstream fileout("stage1.txt");
    for(int i=0; i<stage1_outputs.size(); i++)
    {
        if(i%3==0)
        {
            fileout<<endl;
        }
        Point ansPoint=stage1_outputs[i];
        fileout<< setprecision(7)<< fixed<<ansPoint.x<<" "<<ansPoint.y<<" "<<ansPoint.z<<endl;
    }
    fileout.close();
}

int main()
{
    FILE* file1 = freopen("scene.txt","r",stdin);
    cin>>  eyeX>> eyeY>> eyeZ>> lookX>> lookY>> lookZ>> upX>> upY>> upZ>> fovY>> aspectRatio>> near>> far;
    Point eye(eyeX, eyeY, eyeZ);
    Point look(lookX, lookY, lookZ);
    Point up(upX, upY, upZ);
    string s;
    cin>>s;
    localStack.push(identity_vec);
    while(s!="end")
    {
        if(s=="triangle")
        {
            //cout<<"triangle found"<<endl;
            calcFinalMatrix();
            double x,y,z;
            for(int i=1; i<=3; i++)
            {
                cin>>x>>y>>z;
                Point p(x,y,z);
                Point ansPoint = matrixPointMultiplication(p);
                stage1_outputs.push_back(ansPoint);
            }
            cin>>s;

        }
        else if(s=="translate")
        {
            double x,y,z;
            cin>>x>>y>>z;
            calcTranslateMatrix(x, y, z);
            cin>>s;
        }
        else if(s=="scale")
        {
            double x,y,z;
            cin>>x>>y>>z;
            calcScalingMatrix(x, y, z);
            cin>>s;
        }
        else if(s=="rotate")
        {
            double angle,x,y,z;
            cin>>angle>>x>>y>>z;
            cin>>s;
            Point a(x,y,z);
            Point c1 = RodriguesFormula(unit_i, a, angle);
            Point c2 = RodriguesFormula(unit_j, a, angle);
            Point c3 = RodriguesFormula(unit_k, a, angle);
            calcRotationMatrix(c1, c2, c3);
        }
        else if(s=="push")
        {
            cin>>s;
            int now_indicator = localStack.size();
            stack<vector<double>>copy_local_stack = localStack;
            stack<vector<double>>local_stack2;
            while(copy_local_stack.empty()==false)
            {
                local_stack2.push(copy_local_stack.top());
                copy_local_stack.pop();
            }
            while(local_stack2.empty()==false)
            {
                globalStack.push(local_stack2.top());
                local_stack2.pop();
            }
            globalStack_indicator.push(now_indicator);
        }
        else if(s=="pop")
        {
            cin>>s;
            int now=globalStack_indicator.top();
            globalStack_indicator.pop();
            stack<vector<double>>local_stack;
            for(int i=1; i<=now; i++)
            {
                local_stack.push(globalStack.top());
                globalStack.pop();
            }
            while(localStack.empty()==false)
            {
                localStack.pop();
            }
            while(local_stack.empty()==false)
            {
                localStack.push((local_stack.top()));
                local_stack.pop();
            }
        }
        else
        {
            break;
        }
    }
    fclose(file1);

    printStage1Output();
    stage2(eye, look, up);
    stage3();


    int screen_width=0, screen_height=0;
    ifstream myfile("config.txt");
    myfile>>screen_width;
    myfile>>screen_height;
    myfile.close();

    double box_left=-1.0, box_right=1.0, box_bottom=-1.0, box_top=1.0;
    double Z_min=-1.0, Z_max=1.0;
    double dx = (box_right-box_left)/screen_width;
    double dy = (box_top-box_bottom)/screen_height;
    double Top_Y = box_top - dy/2.0;
    double Left_X = box_left + dx/2.0;


    // initialize z-buffer and frame buffer

    vector< vector<double> > z_buffer(screen_height, vector<double>(screen_width, Z_max));
    for(int i=0; i<screen_height; i++)
    {
        for(int j=0; j<screen_width; j++)
        {
            z_buffer[i][j] = Z_max;
        }
    }

    bitmap_image image(screen_width, screen_height);
    image.set_all_channels(0, 0, 0);

    vector<int>colors;
    for(int i=0; i<stage3_outputs.size(); i+=3)
    {
        Point a = stage3_outputs[i];
        Point b = stage3_outputs[i+1];
        Point c = stage3_outputs[i+2];



        //triangle.sort();
        if( b.y < a.y )
            swap(a,b);
        if( c.y < a.y )
            swap(a,c);
        if( c.y < b.y )
            swap(b,c);
        //triangle.recolor();
        int red = random();
        int green = random();
        int blue = random();
        colors.push_back(red);
        colors.push_back(green);
        colors.push_back(blue);

        // clipping
        double min_y = a.y;
        double max_y = b.y;

        min_y = max(min_y, box_bottom);
        max_y = min(max_y, box_top);

        // scan conversion

        //cout<<"y range "<<min_y<<" "<<max_y<<endl;
        for(double y=min_y; y<=max_y; y+=dy)
        {

            // compute min_x and max_x for this row
            double min_x = 0;
            double max_x = -1;
            double min_z = 0;
            double max_z = -1;


            if( b.y != a.y and a.y != c.y )
            {
                min_x = a.x + (b.x-a.x)*(y-a.y)/(b.y-a.y);
                max_x = a.x + (c.x-a.x)*(y-a.y)/(c.y-a.y);

                min_z = a.z + (b.z-a.z)*(y-a.y)/(b.y-a.y);
                max_z = a.z + (c.z-a.z)*(y-a.y)/(c.y-a.y);

                if( min_x > max_x )
                {
                    swap(min_x, max_x);
                    swap(min_z, max_z);
                }
            }

            min_x = max(min_x, box_left);
            max_x = min(max_x, box_right);

            for(double x=min_x; x<=max_x; x+=dx)
            {
                if( max_x == min_x )
                    continue;

                int i = (Top_Y-y)/dy;
                int j = (x-Left_X)/dx;

                double z = min_z + (max_z-min_z)*(x-min_x)/(max_x-min_x);
                if( z < z_buffer[i][j] and z>Z_min )
                {
                    z_buffer[i][j] = z;
                    image.set_pixel(j, i, red, green, blue);
                }

            }
        }


        min_y = b.y;
        max_y = c.y;
        min_y = max(min_y, box_bottom);
        max_y = min(max_y, box_top);

        // scan conversion
        for(double y=min_y; y<=max_y; y+=dy)
        {

            // compute min_x and max_x for this row
            double min_x = 0;
            double max_x = -1;
            double min_z = 0;
            double max_z = -1;


            if( b.y != c.y and a.y != c.y )
            {
                min_x = c.x + (b.x-c.x)*(y-c.y)/(b.y-c.y);
                max_x = a.x + (c.x-a.x)*(y-a.y)/(c.y-a.y);

                min_z = c.z + (b.z-c.z)*(y-c.y)/(b.y-c.y);
                max_z = a.z + (c.z-a.z)*(y-a.y)/(c.y-a.y);

                if( min_x > max_x )
                {
                    swap(min_x, max_x);
                    swap(min_z, max_z);
                }
            }
            min_x = max(min_x, box_left);
            max_x = min(max_x, box_right);

            for(double x=min_x; x<=max_x; x+=dx)
            {
                if( max_x == min_x )
                    continue;

                int i = (Top_Y-y)/dy;
                int j = (x-Left_X)/dx;

                double z = min_z + (max_z-min_z)*(x-min_x)/(max_x-min_x);
                if( z < z_buffer[i][j] and z>Z_min )
                {
                    z_buffer[i][j] = z;
                    image.set_pixel(j, i, red, green, blue);
                }

            }
        }

    }

    image.save_image("Myoutput.bmp");
    ofstream fileout("z_buffer.txt");
    for(int i=0; i<screen_height; i++)
    {
        for(int j=0; j<screen_width; j++)
        {
            if (z_buffer[i][j] < Z_max)
            {
                fileout << setprecision(6) << fixed << z_buffer[i][j] << "\t";
            }
        }
        fileout<<endl;
    }
    fileout.close();
    z_buffer.clear();
    image.clear();
    z_buffer.shrink_to_fit();
    return 0;

}
