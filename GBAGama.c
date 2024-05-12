typedef volatile unsigned short hword;
#define WHITE     0x7FFF
#define BLACK     0x0000
#define RED       0x001F
#define GREEN     0x03E0
#define BLUE      0x7C00
#define YELLOW    0x03FF
#define MAGENTA   0x7C1F
#define CYAN      0x7FE0
#define VRAM (hword *) 0x06000000
#define KEY_NONE        0x0000        //何も押してない時
#define KEY_A           1<<0        //A
#define KEY_B           1<<1        //B
#define KEY_SELECT      1<<2        //SELECT
#define KEY_START       1<<3        //START
#define KEY_RIGHT       1<<4        //→
#define KEY_LEFT        1<<5        //←
#define KEY_UP          1<<6        //↑
#define KEY_DOWN        1<<7        //↓

#include    "8x8ascii.h"

//x,y座標の基準となるグローバル変数
hword sx = 0; 
hword sy = 0;

//locate関数でのx,y座標を扱うグローバル変数
typedef struct {
    hword x;
    hword y;
}point;

point p;

//プレイヤーが動かすオブジェクトの中心座標を扱うグローバル変数
hword obj_x = 35;   //上下のみの移動のためx座標は固定
hword obj_y;

//各大砲から発射される弾の中心座標を扱うグローバル変数
hword cannon_x1;
hword cannon_y1;
hword cannon_x2;
hword cannon_y2;
hword cannon_x3;
hword cannon_y3;
hword cannon_x4;
hword cannon_y4;

//どの大砲から弾が出てくるかを決める変数
hword cannon_num;

//各大砲の軌道上に弾がいくつあるかを定める変数
hword circle_num1 = 0;
hword circle_num2 = 0;
hword circle_num3 = 0;
hword circle_num4 = 0;

//大砲の弾が画面上にトータルでいくつあるかを定めておく変数
hword total_bullet;

//いくつ避けたかを判定する変数
hword score;

volatile unsigned short time;

void draw_point(hword, hword,hword);
void prints(unsigned char *str,hword color);
void printn(hword,hword color);
void locate(hword,hword);
void print_ascii(unsigned char,hword color);
hword mod(hword dividened, hword divisor);
hword div(hword dividened, hword divisor);
void make_square(hword x1, hword x2, hword y1, hword y2, hword color);
void make_circle(hword xe, hword ye, hword R, hword color);
int check_color(hword x,hword y);
hword hit_check(hword x,hword y);
hword hit_checkp(hword x,hword y);
void timerON();
void timerOFF();
void timerInitial();

int main(void){
	hword *ptr,*ptr2;
    hword i,j,key1,key2,tmp,check_point,hit;
    hword tx, ty;
    hword x1,y1;
    int count;
    /* GBAモード設定初期化 */
    ptr = (hword*) 0x04000000; /* モード設定アドレス*/ 
    *ptr = 0x0F03;    /* モード設定の初期化*/ 

    ptr = (hword *)0x06000000;
    ptr2 = (hword *)0x04000130;

    /********ルール説明********/
    locate(7,9);
    prints("Dodge Bullet GAME",WHITE);

    locate(7,11);
    prints("NEXT:Press",WHITE);
    locate(18,11);
    prints("START",GREEN);
    while(1){
        ptr2 = (hword *)0x04000130;
        key1 = ~(*ptr2) & 0x03FF;
        if(key1 == KEY_START){
            break;
        }
    }

    for(i = 0;i < 240;i++){
        for(j = 0;j < 160; j++){
            draw_point(i,j,BLACK);
        }
    }
    locate(13,2);
    prints("RULE",WHITE);

    locate(7,9);
    prints("Dodge 20 bullets",WHITE);
    locate(3,10);
    prints("If hit bulett",WHITE);
    prints("GAMEORVER",BLUE);

    locate(7,13);
    prints("NEXT:Press",WHITE);
    locate(18,13);
    prints("START",GREEN);
    while(1){
        ptr2 = (hword *)0x04000130;
        key1 = ~(*ptr2) & 0x03FF;
        if(key1 == KEY_START){
            break;
        }
    }

    for(i = 0;i < 240;i++){
        for(j = 0;j < 160; j++){
            draw_point(i,j,BLACK);
        }
    }

    locate(8,2);
    prints("HOW TO OPERATE",WHITE);

    locate(7,9);
    prints("Press KEY_UP:up",WHITE);
    locate(5,10);
    prints("Press KEY_DOWN:down",WHITE);

    locate(7,13);
    prints("NEXT:Press",WHITE);
    locate(18,13);
    prints("START",GREEN);

    while(1){
        ptr2 = (hword *)0x04000130;
        key1 = ~(*ptr2) & 0x03FF;
        if(key1 == KEY_START){
            break;
        }
    }

    for(i = 0;i < 240;i++){
        for(j = 0;j < 160; j++){
            draw_point(i,j,BLACK);
        }
    }

    locate(7,9);
    prints("Are You Ready??",WHITE);
    locate(8,13);
    prints("Press",WHITE);
    locate(15,13);
    prints("START!",GREEN);
    while(1){
        ptr2 = (hword *)0x04000130;
        key1 = ~(*ptr2) & 0x03FF;
        if(key1 == KEY_START){
            break;
        }
    }

    for(i = 0;i < 240;i++){
        for(j = 0;j < 160; j++){
            draw_point(i,j,BLACK);
        }
    }

    locate(10,9);
    prints("SATRT!!!!",WHITE);
    //いきなり始まらないようにする
    for(count = 0; count < 1000000; count++){}
    for(i = 0;i < 240;i++){
        for(j = 0;j < 160; j++){
            draw_point(i,j,BLACK);
        }
    }
    /***************************/

    obj_x = 35;
    obj_y = 18;
    //プレーヤーが操作するオブジェクトの初期位置
    make_square(20,50,3,33,WHITE);
    //大砲4つ
    make_square(209,239,8,28,RED);
    make_square(209,239,48,68,RED);
    make_square(209,239,88,108,RED);
    make_square(209,239,128,148,RED);

    //乱数を生成するために時間を測る
    //測った時間を10で割ったあまりを求め、疑似的に0～9の乱数を生成する
    timerInitial();
	timerON();

    for(count = 0; count < 500000; count++){}
    while(1){
        //規定の個数避けたらループを抜ける
        if(score >= 20){
            break;
        }
        //移動時に弾にぶつかったらループを抜ける
        if(hit == 1){
            break;
        }
        //乱数によってどの大砲からの弾かを決める
        time = 	*((unsigned short *)0x04000108) - 0xFDA8;
        tmp = mod(time,10);
        cannon_num = tmp;

        //弾生成
        //軌道上に弾がなくてかつ画面上に自分以外で弾が2つまででその大砲が乱数によって選択された時
        if(circle_num1 == 0 && (cannon_num == 0 || cannon_num == 1) && total_bullet <= 2){
            //画面全体の弾の数を足す1する
            total_bullet += 1;
            //大砲1の軌道上に弾があることを示すために1にする
            circle_num1 += 1;
            //初期位置に弾表示
            cannon_x1 = 199;
            cannon_y1 = 18;
            make_circle(cannon_x1,cannon_y1,10,RED);
        }
        if(circle_num2 == 0 && (cannon_num == 2 || cannon_num == 3 || cannon_num == 4) && total_bullet <= 2){
            //画面全体の弾の数を足す1する
            total_bullet += 1;
            //大砲1の軌道上に弾があることを示すために1にする
            circle_num2 += 1;
            //初期位置に弾表示
            cannon_x2 = 199;
            cannon_y2 = 58;
            make_circle(cannon_x2,cannon_y2,10,RED);
        }
        if(circle_num3 == 0 && (cannon_num == 5 || cannon_num == 6 || cannon_num == 7) && total_bullet <= 2){
            //画面全体の弾の数を足す1する
            total_bullet += 1;
            //大砲1の軌道上に弾があることを示すために1にする
            circle_num3 += 1;
            //初期位置に弾表示
            cannon_x3 = 199;
            cannon_y3 = 98;
            make_circle(cannon_x3,cannon_y3,10,RED);
        }
        if(circle_num4 == 0 && (cannon_num == 8 || cannon_num == 9) && total_bullet <= 2){
            //画面全体の弾の数を足す1する
            total_bullet += 1;
            //大砲1の軌道上に弾があることを示すために1にする
            circle_num4 += 1;
            //初期位置に弾表示
            cannon_x4 = 199;
            cannon_y4 = 138;
            make_circle(cannon_x4,cannon_y4,10,RED);
        }

        //弾を動かす
        //大砲1の弾
        if(circle_num1 == 1){
            //現在の位置にある弾を削除する
            make_circle(cannon_x1,cannon_y1,10,BLACK);
            //位置を移動させる
            cannon_x1 -= 2;
            //プレイヤーとぶつかったかどうか判定しぶつかったならゲームオーバーとしてループを抜ける
            hit = hit_check(cannon_x1,cannon_y1);
            if(hit == 1){
                break;
            }
            //もし端に到達したら消す
            check_point = cannon_x1;
            if((check_point - 10) <= 0){
                //消して
                make_circle(cannon_x1,cannon_y1,10,BLACK);
                //軌道上に弾がないことにする
                circle_num1 = 0;
                //座標も元に戻す
                cannon_x1 = 199;
                cannon_y1 = 18;
                //画面全体の弾の数も引く1
                total_bullet -= 1;
                //避けた数＋1
                score += 1;
            }else{//それ以外は普通に移動させる
                make_circle(cannon_x1,cannon_y1,10,RED);
            }
        }
        
        //大砲3の弾
        if(circle_num2 == 1){
            //現在の位置にある弾を削除する
            make_circle(cannon_x2,cannon_y2,10,BLACK);
            //位置を移動させる
            cannon_x2 -= 2;
            //プレイヤーとぶつかったかどうか判定しぶつかったならゲームオーバーとしてループを抜ける
            hit = hit_check(cannon_x2,cannon_y2);
            if(hit == 1){
                break;
            }
            //もし端に到達したら消す
            check_point = cannon_x2;
            if((check_point - 10) <= 0){
                //消して
                make_circle(cannon_x2,cannon_y2,10,BLACK);
                //軌道上に弾がないことにする
                circle_num2 = 0;
                //座標も元に戻す
                cannon_x2 = 199;
                cannon_y2 = 58;
                //画面全体の弾の数も引く1
                total_bullet -= 1;
                //避けた数＋1
                score += 1;
            }else{  //それ以外は普通に移動させる
                make_circle(cannon_x2,cannon_y2,10,RED);
            }
        }

        //大砲3の弾
        if(circle_num3 == 1){
            //現在の位置にある弾を削除する
            make_circle(cannon_x3,cannon_y3,10,BLACK);
            //位置を移動させる
            cannon_x3 -= 2;
            //プレイヤーとぶつかったかどうか判定しぶつかったならゲームオーバーとしてループを抜ける
            hit = hit_check(cannon_x3,cannon_y3);
            if(hit == 1){
                break;
            }
            //もし端に到達したら消す
            check_point = cannon_x3;
            if((check_point - 10) <= 0){
                //消して
                make_circle(cannon_x3,cannon_y3,10,BLACK);
                //軌道上に弾がないことにする
                circle_num3 = 0;
                //座標も元に戻す
                cannon_x3 = 199;
                cannon_y3 = 98;
                //画面全体の弾の数も引く1
                total_bullet -= 1;
                //避けた数＋1
                score += 1;
            }else{  //それ以外は普通に移動させる
                make_circle(cannon_x3,cannon_y3,10,RED);
            }
        }

        //大砲4の弾
        if(circle_num4 == 1){
            //現在の位置にある弾を削除する
            make_circle(cannon_x4,cannon_y4,10,BLACK);
            //位置を移動させる
            cannon_x4 -= 2;
            //プレイヤーとぶつかったかどうか判定しぶつかったならゲームオーバーとしてループを抜ける
            hit = hit_check(cannon_x4,cannon_y4);
            if(hit == 1){
                break;
            }
            //もし端に到達したら消す
            check_point = cannon_x4;
            if((check_point - 10) <= 0){
                //消して
                make_circle(cannon_x4,cannon_y4,10,BLACK);
                //軌道上に弾がないことにする
                circle_num4 = 0;
                //座標も元に戻す
                cannon_x4 = 199;
                cannon_y4 = 138;
                //画面全体の弾の数も引く1
                total_bullet -= 1;
                //避けた数＋1
                score += 1;
            }else{  //それ以外は普通に移動させる
                make_circle(cannon_x4,cannon_y4,10,RED);
            }
        }
        

        ptr2 = (hword *)0x04000130;
        key1 = ~(*ptr2) & 0x03FF;

        switch(key1){
            case KEY_UP:
            for(count = 0;count < 100000;count++){//遅延
            }

            //現在地にあるオブジェクトを消す
            x1 = obj_x;
            y1 = obj_y;
            make_square((x1-15),(x1+15),(y1-15),(y1+15),BLACK);
            tmp = obj_y;
            if((tmp-40) >= 18){
                obj_y = obj_y - 40;//上方向に座標を変更し
            }
            //移動先の座標に弾の色(RED)があるかを判定して、ぶつかったならゲームオーバーとしてループを抜ける
            //こっちの条件も弾の移動方法を今の位置を黒→次の位置赤としておりかつ高速で動作させている為か、黒の瞬間に移動するとぶつかった判定にならない
            //かといってこれ以外の方法で弾を移動させる方法が思いつかなかった上、
            //色が高速で切り替わらないようにするとゲームとして簡単になり過ぎてしまうため今回はこのままとした
            hit = hit_checkp(obj_x,obj_y);
            if(hit == 1){
                break;
            }
            //弾の方がプレイヤーよりも小さいので移動先の座標に弾の端が含まれるかどうかで当たったかを判定し、ぶつかったならゲームオーバーとしてループを抜ける
            /*if((( (obj_x + 15) <= (cannon_x1 + 10) || (obj_x - 15) >= (cannon_x1 + 10) ) && (obj_y == cannon_y1)) || 
            (( (obj_x + 15) <= (cannon_x1 - 10) || (obj_x - 15) >= (cannon_x1 - 10) ) && (obj_y == cannon_y1)) || 
            (( (obj_x + 15) <= (cannon_x2 + 10) || (obj_x - 15) >= (cannon_x2 + 10) ) && (obj_y == cannon_y2)) || 
            (( (obj_x + 15) <= (cannon_x2 - 10) || (obj_x - 15) >= (cannon_x2 - 10) ) && (obj_y == cannon_y2)) ||
            (( (obj_x + 15) <= (cannon_x3 + 10) || (obj_x - 15) >= (cannon_x3 + 10) ) && (obj_y == cannon_y3)) || 
            (( (obj_x + 15) <= (cannon_x3 - 10) || (obj_x - 15) >= (cannon_x3 - 10) ) && (obj_y == cannon_y3)) ||
            (( (obj_x + 15) <= (cannon_x4 + 10) || (obj_x - 15) >= (cannon_x4 + 10) ) && (obj_y == cannon_y4)) || 
            (( (obj_x + 15) <= (cannon_x4 - 10) || (obj_x - 15) >= (cannon_x4 - 10) ) && (obj_y == cannon_y4))){
                hit = 1;
                break;
            }*/ //上手くいきませんでした
            if(obj_y < 0){    //一番上の定位置よりも座標が小さくなったらそこで停止させる
                obj_y = 18;
                x1 = obj_x;
                y1 = obj_y;
                make_square((x1-15),(x1+15),(y1-15),(y1+15),WHITE);
            }else{  //それ以外は普通に上方向にオブジェクトを動かす
                x1 = obj_x;
                y1 = obj_y;
                make_square((x1-15),(x1+15),(y1-15),(y1+15),WHITE);
            }
            break;

            case KEY_DOWN:
            for(count = 0;count < 100000;count++){//遅延
            }

            //現在地にあるオブジェクトを消す
            x1 = obj_x;
            y1 = obj_y;
            make_square((x1-15),(x1+15),(y1-15),(y1+15),BLACK);

            tmp = obj_y;
            if((tmp+40) <= 138){
                obj_y = obj_y + 40;//下方向に座標を変更し
            }
            
            //移動先の座標に弾の色(RED)があるかを判定して、ぶつかったならゲームオーバーとしてループを抜ける
            //こっちの条件も弾の移動方法を今の位置を黒→次の位置赤としておりかつ高速で動作させている為か、黒の瞬間に移動するとぶつかった判定にならない
            //かといってこれ以外の方法で弾を移動させる方法が思いつかなかった上、
            //色が高速で切り替わらないようにするとゲームとして簡単になり過ぎてしまうため今回はこのままとした
            hit = hit_checkp(obj_x,obj_y);
            if(hit == 1){
                break;
            }
            //弾の方がプレイヤーよりも小さいので移動先の座標に弾の端が含まれるかどうかで当たったかを判定し、ぶつかったならゲームオーバーとしてループを抜ける
            /*if((( (obj_x + 15) <= (cannon_x1 + 10) || (obj_x - 15) >= (cannon_x1 + 10) ) && (obj_y == cannon_y1)) || 
            (( (obj_x + 15) <= (cannon_x1 - 10) || (obj_x - 15) >= (cannon_x1 - 10) ) && (obj_y == cannon_y1)) || 
            (( (obj_x + 15) <= (cannon_x2 + 10) || (obj_x - 15) >= (cannon_x2 + 10) ) && (obj_y == cannon_y2)) || 
            (( (obj_x + 15) <= (cannon_x2 - 10) || (obj_x - 15) >= (cannon_x2 - 10) ) && (obj_y == cannon_y2)) ||
            (( (obj_x + 15) <= (cannon_x3 + 10) || (obj_x - 15) >= (cannon_x3 + 10) ) && (obj_y == cannon_y3)) || 
            (( (obj_x + 15) <= (cannon_x3 - 10) || (obj_x - 15) >= (cannon_x3 - 10) ) && (obj_y == cannon_y3)) ||
            (( (obj_x + 15) <= (cannon_x4 + 10) || (obj_x - 15) >= (cannon_x4 + 10) ) && (obj_y == cannon_y4)) || 
            (( (obj_x + 15) <= (cannon_x4 - 10) || (obj_x - 15) >= (cannon_x4 - 10) ) && (obj_y == cannon_y4))){
                hit = 1;
                break;
            }*/ //上手くいきませんでした
            if(obj_y > 138){ //一番下の定位置よりも座標が大きくなったらそこで停止させる
                obj_y = 138;
                x1 = obj_x;
                y1 = obj_y;
                make_square((x1-15),(x1+15),(y1-15),(y1+15),WHITE);
            }else{  //それ以外は普通に下方向にオブジェクトを動かす
                x1 = obj_x;
                y1 = obj_y;
                make_square((x1-15),(x1+15),(y1-15),(y1+15),WHITE);
            }
            break;

        }
    }

    //20個避けれたら”GAME CLEAR!!!”と表示
    if(score >= 20){
        for(i = 0;i < 240; i++){
            for(j = 0;j < 160; j++){
                draw_point(i,j,BLACK);
            }
        }
        locate(8,9);
        prints("GAME CLEAR!!!",RED);
    }

    //弾に当たったら”GAME ORVER!!!”と表示
    if(hit == 1){
        for(i = 0;i < 240; i++){
            for(j = 0;j < 160; j++){
                draw_point(i,j,BLACK);
            }
        }
        locate(8,9);
        prints("GAME ORVER!!!",BLUE);
    }
    
    //プログラム停止
    while(1);
    return 0;
}

//任意の座標に任意の色で点を描画する関数
void draw_point (hword x,hword y ,hword color){
	hword *ptr;
    ptr = (hword*)VRAM;
	ptr = VRAM + 240*y + x;
	*ptr = color;
}

void locate(hword x,hword y){
    //x,y座標を変更する(30x20各エリアの左上の座標)
    p.x = x*8;
    p.y = y*8;
}

void print_ascii(unsigned char ascii_num,hword color){  //現在地に文字描画をする関数
    hword i,j,check,checkbit;

    for(i = 0;i < 8; i++){
            checkbit = 0x80;
            for(j = 0;j < 8; j++){
                check = char8x8[ascii_num][i] & checkbit;

                if(check != checkbit){
                    draw_point(j + p.x,i + p.y,BLACK);
                }else if(check == checkbit){
                    draw_point(j + p.x,i + p.y,color);
                }

                checkbit = checkbit >> 1;
            }
        }

}


void prints(unsigned char *str,hword color){
    //特定の文字列描画
    while(*str){
        print_ascii(*str++,color);
        p.x += 8;
    }
    
}

/*******************gba_io1.cより引用******************/
void printn(hword val,hword color) {

	unsigned char char_data[] = "0123456789";
	unsigned char buf[6];
	hword tmp;
	int i;
	
	tmp = val;
	for (i = 0; i < 5; i++) {
		if (tmp >= 10){
			tmp = div(tmp, 10);
		} else {
			break;
		}
	}

	buf[i+1] = 0;
	
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}
	
	prints(buf,color);
	
	return;
}

hword div(hword dividened, hword divisor){
	
	hword quotient = 0;		
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return quotient;
}

hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return dividened;
}
/******************************************************/

void make_square(hword x1, hword x2, hword y1, hword y2, hword color){
    hword i,j;
    for(i = x1; i < x2; i++){
        for(j = y1; j < y2; j++){
            draw_point(i, j, color);
        }
    }
}

void make_circle(hword xs, hword ys, hword R, hword color){
    hword i,j;
    for(i = xs - R; i < xs + R; i++){
        for(j = ys - R; j < ys + R; j++){
            if(R*R >= (i-xs)*(i-xs)+(j-ys)*(j-ys)){
            draw_point(i, j, color);
            }
        }
    }
}

//弾がプレイヤーに当たったかどうか判定
hword hit_check(hword x,hword y){
    hword hit;
    if(check_color(x,y) == WHITE || check_color(x+10,y) == WHITE || check_color(x-10,y) == WHITE){
        hit = 1;
    }else {
        hit = 0;
    }
    
    return hit;
}

//プレイヤーが弾に当たったかどうか判定
hword hit_checkp(hword x,hword y){
    hword hit,i,tmp;
    tmp = (x-15);
    for(i = 0; i <= 30;i++){
        if(check_color((tmp + i),y) != RED){
            hit = 0;
        }else if(check_color((tmp + i),y) == RED){
            hit = 1;
        }
    }
    
    return hit;
}


//移動先の色を確認
int check_color(hword x,hword y){
    hword *ptr;
    ptr = (hword*)(0x06000000 + 240*y*2 + x*2);
    return *ptr;
}

//一個上のクラスの人にGBA上での時間の扱い方を聞いて書きました
void timerON(){
    *((unsigned short *)0x04000102) = 0x0080;	
	*((unsigned short *)0x04000106) = 0x0084;		
	*((unsigned short *)0x0400010A) = 0x0084;	
	*((unsigned short *)0x0400010E) = 0x0084;
}
void timerOFF(){
    *((unsigned short *)0x04000102) = 0x0040;	
}
void timerInitial(){
    *((unsigned short *)0x04000102) = 0x0040;	
    *((unsigned short *)0x04000106) = 0x0004;		
    *((unsigned short *)0x0400010A) = 0x0004;	
    *((unsigned short *)0x0400010E) = 0x0004;
    
    *((unsigned short *)0x04000100) = 0x8000;	
	*((unsigned short *)0x04000104) = 0xFFCC;	
	*((unsigned short *)0x04000108) = 0xFDA8;	
	*((unsigned short *)0x0400010C) = 0x0000;	
}