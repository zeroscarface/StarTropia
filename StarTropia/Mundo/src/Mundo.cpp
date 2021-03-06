#include "../include/Mundo.h"

extern int Height;
extern float FPS;
extern bool en_pause;
extern bool en_juego;

//Incializacion de las variables del mundo
Mundo::Mundo(){
	glListBase(0);
	lista = glGenLists(1);


//Variables del sonido
	ALfloat listenerPos[]={0.0,0.0,-5.0};
	ALfloat listenerVel[]={0.0,0.0,0.0};
	ALfloat	listenerOri[]={0.0,0.0,1.0,0.0,1.0,0.0};

	ALfloat source0Pos[]={ 0.0, 0.0, 0.0};
	ALfloat source0Vel[]={ 0.0, 0.0, 0.0};

	alutInit(0, NULL);

    	alListenerfv(AL_POSITION,listenerPos);
    	alListenerfv(AL_VELOCITY,listenerVel);
    	alListenerfv(AL_ORIENTATION,listenerOri);

	alGetError(); 

	// Defino numero de buffers
	alGenBuffers(NUM_BUFFERS, bufferS);

	ALboolean  al_bool;

	//cargar musica de fondo
	alutLoadWAVFile((ALbyte *) "Music/Inertia.wav", &format, &data, &size,&freq, &al_bool);
    	alBufferData(bufferS[0],format,data,size,freq);
	alutUnloadWAV(format,data,size,freq);
	
	//cargar musica de laser
	alutLoadWAVFile((ALbyte *) "Music/laser2.wav", &format, &data, &size,&freq, &al_bool);
    	alBufferData(bufferS[1],format,data,size,freq);
	alutUnloadWAV(format,data,size,freq);

	//cargar musica de explosion
	alutLoadWAVFile((ALbyte *) "Music/bum.wav", &format, &data, &size,&freq, &al_bool);
    	alBufferData(bufferS[2],format,data,size,freq);
	alutUnloadWAV(format,data,size,freq);


	alGetError();
	alGenSources(NUM_SOURCES, source);

	//Definicion de la fuente de sonido de fondo
    	alSourcef(source[0],AL_PITCH,1.0f);
    	alSourcef(source[0],AL_GAIN,1.0f);
    	alSourcefv(source[0],AL_POSITION,source0Pos);
    	alSourcefv(source[0],AL_VELOCITY,source0Vel);
    	alSourcei(source[0],AL_BUFFER,bufferS[0]);
    	alSourcei(source[0],AL_LOOPING,AL_TRUE);

	//Definicion de la fuente de sonido del laser
    	alSourcef(source[1],AL_PITCH,1.0f);
    	alSourcef(source[1],AL_GAIN,1.0f);
    	alSourcefv(source[1],AL_POSITION,source0Pos);
    	alSourcefv(source[1],AL_VELOCITY,source0Vel);
    	alSourcei(source[1],AL_BUFFER,bufferS[1]);
    	alSourcei(source[1],AL_LOOPING,AL_FALSE);

	//Definicion de la fuente de sonido de la explosion
    	alSourcef(source[2],AL_PITCH,1.0f);
    	alSourcef(source[2],AL_GAIN,1.0f);
    	alSourcefv(source[2],AL_POSITION,source0Pos);
    	alSourcefv(source[2],AL_VELOCITY,source0Vel);
    	alSourcei(source[2],AL_BUFFER,bufferS[2]);
    	alSourcei(source[2],AL_LOOPING,AL_FALSE);
	
    glNewList( lista + 0 , GL_COMPILE );
		renderString(-0.5,  1.5,-1,GLUT_BITMAP_TIMES_ROMAN_24, "STARTROPIA");
		renderString( 0  ,-0.25,-1,GLUT_BITMAP_HELVETICA_18, "Play");
		renderString(-1  , -1.5,-1,GLUT_BITMAP_HELVETICA_18, "Last Score: ");	
    glEndList();
	
    entorno = new c_entorno( 2,2,"Resources/Cube_Maps/nivel1/" );

	Nave = new Jugador(0,0,0,0,0);
	Cam = new Camara(Nave);
	luz = new c_luz[1];
	
    luz[0].position( 1.0f,  1.0f, 1.0f );
    luz[0].diffuse(  1.0f,  1.0f, 1.0f );
    luz[0].ambient(   0.0f,  0.0f, 0.0f );
    luz[0].direccional( GL_LIGHT0 );	
    glEnable( GL_LIGHT0);	
	
    material = new c_material[2]; //2 Materiales
    
    material[0].diffuse(  0.8f, 0.8f, 0.8f );
    material[0].ambient(  0.2f, 0.2f, 0.2f );
    material[0].specular( 0.5f, 0.5f, 0.5f );
    material[0].phongsize( 128.0f );
    
    material[1].diffuse(  0.9f, 0.7f, 0.1f );
    material[1].ambient(  0.9f, 0.7f, 0.1f );
    material[1].specular( 0.9f, 0.7f, 0.1f );
    material[1].phongsize( 128.0f );

    // -- TRES PUNTOS DEL SUELO EN SENTIDO ANTIHORARIO -------------------------
    GLfloat Puntos[9] = { -1, -2.3, -1,   -1, -2.3,  1,   1, -2.3,  1  };
    Ecuacion_del_Plano( Puntos, Plano );


	int i;
	for(i=0; i<Max_Target ;++i)
		Targets[i] = new Target(i);
	
	for(i=0; i<Max_Anillo ;++i)
		Anillos[i] = new Anillo();
	
	for(i=0; i<Max_Edificio ;++i)
		Edificios[i] = new Edificio(i%3);

	in_Menu = true;
	TimeG = 0.0f;
	TimeAC = 0.0f;
	Time_A = 0.0005f;
	TimeB = 0.0f;
	TimeBC = 0.0f;
	Time_Mete = 0.00075f;
    Time_C  = 0.00065f;
    TimeCC = 0.0f;

	//Inicio de la cancion de fondo
	//alSourcePlay(source[0]);

    Init();
}

Mundo::~Mundo(){


}

int Mundo::AnillosMundo(int i){
    
    int Dezp;
    if(Inicio)
        Dezp = 15 + rand() % 100;
    else
        Dezp = 100;

    Anillos[i]->Assign(rand() % 4 - 1.5,rand() % 4 - 1.5,Nave->z - Dezp);
    bool Tmp = false;
    for(int j=0;j<Max_Anillo && !Tmp;++j)
        if(j!=i)
            Tmp = Anillos[j]->Collision2(Anillos[i]->minx, Anillos[i]->miny, Anillos[i]->minz, Anillos[i]->maxx, Anillos[i]->maxy, Anillos[i]->maxz);

    for(int j=0;j<Max_Target && !Tmp;++j)
        Tmp = Targets[j]->Collision2(Anillos[i]->minx, Anillos[i]->miny, Anillos[i]->minz, Anillos[i]->maxx, Anillos[i]->maxy, Anillos[i]->maxz);
        
    for(int j=0;j<Max_Edificio && !Tmp;++j)
        Tmp = Edificios[j]->Collision2(Anillos[i]->minx, Anillos[i]->miny, Anillos[i]->minz, Anillos[i]->maxx, Anillos[i]->maxy, Anillos[i]->maxz);


    if(Tmp)
        AnillosMundo(i);
    
    return 0;
}

int Mundo::MeteoritosMundo(int i){    

    int Dezp;
    if(Inicio)
        Dezp = 15 + rand() % 100;
    else
        Dezp = 100;

    Targets[i]->Assign(rand() % 4 - 1.5,rand() % 4 - 1.5,Nave->z - Dezp,0,0);
    bool Tmp = false;
    for(int j=0;j<Max_Anillo && !Tmp;++j)
        Tmp = Anillos[j]->Collision2(Targets[i]->minx, Targets[i]->miny, Targets[i]->minz, Targets[i]->maxx, Targets[i]->maxy, Targets[i]->maxz);

    for(int j=0;j<Max_Target && !Tmp;++j)
        if(i!=j)
            Tmp = Targets[j]->Collision2(Targets[i]->minx, Targets[i]->miny, Targets[i]->minz, Targets[i]->maxx, Targets[i]->maxy, Targets[i]->maxz);
        
    for(int j=0;j<Max_Edificio && !Tmp;++j)
        Tmp = Edificios[j]->Collision2(Targets[i]->minx, Targets[i]->miny, Targets[i]->minz, Targets[i]->maxx, Targets[i]->maxy, Targets[i]->maxz);

    if(Tmp)
        MeteoritosMundo(i);
    
    return 0;
}

int Mundo::EdificiosMundo(int i){
    bool Tmp = false;
    
    int Dezp;
    if(Inicio)
        Dezp = 15 + rand() % 100;
    else
        Dezp = 100;

    Edificios[i]->Assign(rand() % 4 - 1.5,-1.25,Nave->z - Dezp);

    for(int j=0;j<Max_Anillo && !Tmp;++j)
        Tmp = Anillos[j]->Collision2(Edificios[i]->minx, Edificios[i]->miny, Edificios[i]->minz, Edificios[i]->maxx, Edificios[i]->maxy, Edificios[i]->maxz);

    for(int j=0;j<Max_Target && !Tmp;++j)
    	Tmp = Targets[j]->Collision2(Edificios[i]->minx, Edificios[i]->miny, Edificios[i]->minz, Edificios[i]->maxx, Edificios[i]->maxy, Edificios[i]->maxz);
    
    for(int j=0;j<Max_Edificio && !Tmp;++j)
        if(i!=j)
            Tmp = Edificios[j]->Collision2(Edificios[i]->minx, Edificios[i]->miny, Edificios[i]->minz, Edificios[i]->maxx, Edificios[i]->maxy, Edificios[i]->maxz);

    if(Tmp)
        EdificiosMundo(i);
    
    return 0;
}

void Mundo::Init(){

    srand ( time(NULL) );

    Inicio =  true;
    for(int j=0;j<Max_Edificio;++j)
        EdificiosMundo(j);  

    for(int j=0;j<Max_Anillo;++j)
        AnillosMundo(j);
    
    for(int j=0;j<Max_Target;++j)
        MeteoritosMundo(j);

    Inicio = false;
}




//Reinicia la partida de juego
void Mundo::ReSpam(){
    Nave->ReSpam(0,0,0);
	TimeG = 0.0f;
	TimeAC = 0.0f;
	TimeBC = 0.0f;
	FPS = 30.0f;
	int i;
	for(i=0; i<Max_Target ;++i)
		Targets[i]->Alive = false;
	
	for(i=0; i<Max_Anillo ;++i)
		Anillos[i]->Alive = false;	

	for(i=0; i<Max_Edificio ;++i)
		Edificios[i]->Alive = false;		

}

//Muestra el Score de la partida
void Mundo::Menu(){
    sprintf(buffer,"%d",Nave->score);
}

//Ejecuta una partida de juego
void Mundo::Run(float Time){
    //std::cout << Nave->x <<","<< Nave->y <<","<< Nave->z << std::endl;
    if(!en_pause){

      	TimeG  += Time - TimeB;


	if (!en_juego){
		TiempoContador += Time - TimeB;		
	} 	
	
	
        if(TiempoContador < 60.0f){
        
            TimeCC += Time - TimeB;
            TimeBC += Time - TimeB;
            TimeAC += Time - TimeB;

            if(TimeBC > Time_Mete){
                CrearMeteorito();
                TimeBC =0;
            }
             	
            if(TimeAC > Time_A){
                CrearAnillo();
                TimeAC =0;
            }
            
            if(TimeCC > Time_C){
                CrearEdificio();
                TimeCC =0;
            }            
            
            Nave->Update(Time);


            int i;
            
            for(i=0; i<Max_Target ;++i){
                if(Targets[i]->Alive)
                    if(Nave->Collision(Targets[i]->minx,Targets[i]->miny,Targets[i]->minz,Targets[i]->maxx,Targets[i]->maxy,Targets[i]->maxz)){
						                        
						Nave->score -= 3;
					}
                    if(Nave->Collision(Targets[i])) {				
						alSourcePlay(source[2]);                        
						Nave->score += 1;
					}            
			}

            for(i=0; i<Max_Anillo ;++i){
                if(Anillos[i]->Alive)
                    if(Anillos[i]->Collision(Nave->minx,Nave->miny,Nave->minz,Nave->maxx,Nave->maxy,Nave->maxz)){
                	    Nave->score += 1;
                    }
            }

            for(i=0; i<Max_Edificio ;++i){
                if(Edificios[i]->Alive)
                    if(Edificios[i]->Collision(Nave->minx,Nave->miny,Nave->minz,Nave->maxx,Nave->maxy,Nave->maxz)){
                        Nave->score -= 5;
                    }
            }

            if (Nave->score < 0)
                Nave->score = 0; 

            for(i=0; i<Max_Target ;++i)
                if(Targets[i]->Alive)
                    Targets[i]->Update(Nave->z,Time);

            for(i=0; i<Max_Anillo ;++i)
                if(Anillos[i]->Alive)
                    Anillos[i]->Update(Nave->z);
                    
            for(i=0; i<Max_Edificio ;++i)
                if(Edificios[i]->Alive)
                    Edificios[i]->Update(Nave->z);

            Cam->Update(Nave);
        }else{
            in_Menu = true;
            Draw_Menu();
        }  
    }
    TimeB = Time;    
	sprintf(buffer,"%d",Nave->score);
	sprintf(buffer1,"%f",FPS);
	sprintf(buffer2,"%f",60.0f - TiempoContador);
	         
}

//Actualiza basado en el tiempo del reloj del sistema
void Mundo::Update(float Time){
    
    if(in_Menu)
        Menu();
    else
        Run(Time);
        
}

//Dibuja las interfaz del score y tiempo de juego
void Mundo::Draw_Menu(){

    Cam->Draw_Zero();

    material[1].activa();
	glPushMatrix();
        glCallList( lista );		
		renderString( 0.5, -1.5,-1,GLUT_BITMAP_HELVETICA_18, buffer);
	glPopMatrix();	
	material[1].desactiva();	
}



void Mundo::Draw_Game(){
	int i;
    luz[0].posiciona(); //Muestra la luz
    
   
    glPushMatrix();
        glTranslatef(Nave->x,Nave->y,Nave->z);
        entorno->mostrar();
    glPopMatrix();  
    
    
    // -- DIBUJAR EL SUELO ---------------------------------------------------
    Hacer_Mascara_Sombra();
    material[0].activa();
    
    
    int k = Nave->z/-200;
    int w = ((int)Nave->z/-100)/2;
    if(((int)Nave->z/-100)%2 == 1)
        ++w;

    glPushMatrix(); //Pares
        glTranslatef(0,0,-200*(k+1)+20);
        entorno->Dibujar_Suelo();
    glPopMatrix();          
        

    glPushMatrix(); //Impares
        glTranslatef(0,0,-100*(2*w+1)+20);
        entorno->Dibujar_Suelo();
    glPopMatrix();
    
    

	material[0].desactiva();
	Terminar_Mascara_Sombra();

	
/*
    // -- DIBUJAR REFLEJOS EN EL SUELO -----------------------------------------
      glPushMatrix();
		glTranslatef(0,-2,0);
        glRotatef( 180, 1.0f, 0.0f, 0.0f );
          material[0].alpha( 0.3f );
          material[0].activa();
            Nave->Draw(); 
          material[0].desactiva();   
          material[0].alpha( 1.0f );
	  glPopMatrix();
*/
	
	
    // -- DIBUJAR LAS SOMBRAS --------------------------------------------------
    if ( glIsEnabled( GL_LIGHT0 ) ) {
      Hacer_Sombra( Plano, luz[0].position() );
        Nave->Draw_Shadow();
	    for(i=0; i<Max_Target ;++i)
		    Targets[i]->Draw_Shadow();
	
	    for(i=0; i<Max_Anillo ;++i)
		    Anillos[i]->Draw_Shadow();        

	    for(i=0; i<Max_Edificio ;++i)
		    Edificios[i]->Draw_Shadow();        

      Terminar_Sombra();
    }

    
    Fin_Sombras();	
	
	// -- DIBUJAR OBJETOS ------------------------------------------------------
	Nave->Draw();

	for(i=0; i<Max_Target ;++i)
		Targets[i]->Draw();
	
	for(i=0; i<Max_Anillo ;++i)
		Anillos[i]->Draw();

	for(i=0; i<Max_Edificio ;++i)
		Edificios[i]->Draw();

    //La parte Escrita en pantalla
	material[1].activa();
	glPushMatrix();
		renderString(Cam->Px+1.4,Cam->Py-2,Cam->Pz-5,GLUT_BITMAP_HELVETICA_18, "Score:");
		renderString(Cam->Px+1.5,Cam->Py-2.2,Cam->Pz-5,GLUT_BITMAP_HELVETICA_18, buffer);

		renderString(Cam->Px-1.7,Cam->Py-2,Cam->Pz-5,GLUT_BITMAP_HELVETICA_18, "FPS:");
		renderString(Cam->Px-1.7,Cam->Py-2.2,Cam->Pz-5,GLUT_BITMAP_HELVETICA_18, buffer1);

		renderString(Cam->Px-0.2,Cam->Py+0.5,Cam->Pz-5,GLUT_BITMAP_HELVETICA_18, "Remaining Time:");
		renderString(Cam->Px-0.2,Cam->Py+0.4,Cam->Pz-5,GLUT_BITMAP_HELVETICA_18, buffer2);		
	glPopMatrix();
	material[1].desactiva();	

}

void Mundo::Draw(){
	Cam->Begin();

    if(in_Menu)
        Draw_Menu();
    else
        Draw_Game();
        
	Cam->End();
}


//Manejo del Keyboard
void Mundo::Keyboard(unsigned char k){
    if(!in_Menu){
	
        Cam->Keyboard(k);
        Nave->Keyboard(k);
    }
}

//Seleccionar elementos con el mouse
void Mundo::mouse(int x,int y){ 
    if(!in_Menu)
        gl_select(x,Height-y);
    else
        if(x < 425 && x > 375 && y < 325 && y > 275){ // << Cuadrado del boton de play
	    TiempoContador = 0.0f;
            //std::cout << "entrando" <<std::endl;
            in_Menu = false;
            ReSpam();
            Init();
        }
}

void Mundo::Reshape(int W, int H){
    Cam->Reshape(W,H);
}


//Generacion de los anillos
void Mundo::CrearAnillo(){
    srand ( time(NULL) );
    
	for(int i=0; i<Max_Anillo ;++i)
		if(!Anillos[i]->Alive)
		{
			AnillosMundo(i);
			break;
		}
}

//Generacion de los Meteoritos
void Mundo::CrearMeteorito(){
    srand ( time(NULL) );
    
	for(int i=0; i<Max_Target ;++i)
		if(!Targets[i]->Alive)
		{
			MeteoritosMundo(i);
			break;
		}
}

//Generacion de los Edificios
void Mundo::CrearEdificio(){
    srand ( time(NULL) );
    
	for(int i=0; i<Max_Edificio ;++i)
		if(!Edificios[i]->Alive)
		{
			Edificios[i]->Assign(rand() % 4 - 1.5,-1.25,Nave->z - 100);
			break;
		}
}

//Muestra en pantalla un String
void Mundo::renderString(float x, float y, float z, void *font, char *string) 
{
    char *c;
    glRasterPos3d(x,y,z);
    for (c=string; *c != '\0'; c++) 
        glutBitmapCharacter(font, *c);
}

//Funcion para el clicking
 void Mundo::gl_select(int x, int y)
 {
 	GLuint buff[64] = {0};
 	GLint hits, view[4];
 	int id;
 
 	glSelectBuffer(64, buff); 
 	glGetIntegerv(GL_VIEWPORT, view);
 
 	glRenderMode(GL_SELECT);
 
 	glInitNames();
 
 	glPushName(0);
 
 	glMatrixMode(GL_PROJECTION);
 	glPushMatrix();
 		glLoadIdentity();
 
 		gluPickMatrix(x, y, 1.0, 1.0, view);
 		Cam->Perspective();
 
 		glMatrixMode(GL_MODELVIEW);
 
 		glutSwapBuffers();
    	for(int i=0; i<Max_Target ;++i)
	    	Targets[i]->Draw();
 
 		glMatrixMode(GL_PROJECTION);
 	glPopMatrix();
 
  	hits = glRenderMode(GL_RENDER);
 
 	list_hits(hits, buff);
 
  	glMatrixMode(GL_MODELVIEW);
 }
 
 //Lista de elementos clickeados
 void Mundo::list_hits(GLint hits, GLuint *names)
 {
 	alSourcePlay(source[1]);
 	if(hits > 0)
	    Nave->Disparar(Targets[(int)names[(hits-1) * 4 + 3]]);	
    	else
	    Nave->Disparar();	    

}
