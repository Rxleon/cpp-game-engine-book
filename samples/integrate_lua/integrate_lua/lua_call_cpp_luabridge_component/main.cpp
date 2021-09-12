extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge\LuaBridge.h"
#include<string>
#include<unordered_map>
#include <stdio.h>
#include <string.h>
#include <list>

using namespace std;

class GameObject;
class Component {
public:
    Component(){};
    virtual ~Component(){};

    GameObject* game_object(){return game_object_;}
    void set_game_object(GameObject* game_object){game_object_=game_object;}

    virtual void Awake(){};
    virtual void Update(){};
private:
    GameObject* game_object_;
};

class Camera:public Component
{
public:
    void Awake() override{
        std::cout<<"Camera Awake"<<std::endl;
    }

    void Update() override{
        std::cout<<"Camera Update"<<std::endl;
    }
};

class Animator:public Component
{
public:
    void Awake() override{
        std::cout<<"Animator Awake"<<std::endl;
    }

    void Update() override{
        std::cout<<"Animator Update"<<std::endl;
    }
};

class GameObject{
public:
    GameObject() {
        game_object_list_.push_back(this);
    }

    ~GameObject() {
    }

    template<typename T>
    T* AddComponent(){
        std::cout<<"GameObject AddComponent:"<<typeid(T).name()<<std::endl;
        T* t=new T();
        Component* component=dynamic_cast<Component*>(t);
        component->set_game_object(this);
        if(component_type_instance_map_.find(typeid(T).hash_code())==component_type_instance_map_.end()){
            std::vector<Component*> component_vec;
            component_vec.push_back(component);
            component_type_instance_map_[typeid(T).hash_code()]=component_vec;
        }else{
            component_type_instance_map_[typeid(T).hash_code()].push_back(component);
        }
        component->Awake();
        return t;
    }

    template<typename T>
    std::vector<Component*>& GetComponents() {
        return component_type_instance_map_[typeid(T).hash_code()];
    }

    template<typename T>
    T* GetComponent() {
        if(component_type_instance_map_.find(typeid(T).hash_code())==component_type_instance_map_.end()){
            return nullptr;
        }
        if(component_type_instance_map_[typeid(T).hash_code()].size()==0){
            return nullptr;
        }
        return dynamic_cast<T*>(component_type_instance_map_[typeid(T).hash_code()][0]);
    }

    void ForeachComponent(std::function<void(Component *)> func) {
        for (auto& v : component_type_instance_map_){
            for (auto& iter : v.second){
                Component* component=iter;
                func(component);
            }
        }
    }


    static void Foreach(std::function<void(GameObject* game_object)> func) {
        for (auto iter=game_object_list_.begin();iter!=game_object_list_.end();iter++){
            auto game_object=*iter;
            func(game_object);
        }
    }
public:
    luabridge::LuaRef AddComponent(luabridge::LuaRef component_type) {
        auto new_table=component_type();//luabridge对c++的class注册为table，并实现了__call，所以可以直接带括号。
        std::cout << typeid(new_table).name() << std::endl;// N9luabridge6LuaRefE
        if(new_table.isInstance<Animator>()) {
            Animator *lua_go = new_table.cast<Animator *>();
        }

        return new_table;
    }

private:
    std::unordered_map<size_t,std::vector<Component*>> component_type_instance_map_;

    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。
};
std::list<GameObject*> GameObject::game_object_list_;



int main(int argc, char * argv[])
{
//    GameObject* go=new GameObject();
//    go->AddComponent<Animator>();
//    go->AddComponent<Camera>();


//    std::cout<<typeid(GameObject).name()<<std::endl;// 10GameObject
    std::cout<<typeid(GameObject).hash_code()<<std::endl;
//    std::cout<<typeid(go).name()<<std::endl;// P10GameObject
//    std::cout<<typeid(Animator).name()<<std::endl;// 8Animator
    std::cout<<typeid(Animator).hash_code()<<std::endl;// 8Animator
//    std::cout<<typeid(animator).name()<<std::endl;// P8Animator

    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);

    luabridge::getGlobalNamespace(lua_state)
            .beginClass<Component>("Component")
            .addConstructor<void (*) ()>()
            .endClass();
    luabridge::getGlobalNamespace(lua_state)
            .beginClass<GameObject>("GameObject")
            .addConstructor<void (*) ()>()
            .addFunction("AddComponent", (luabridge::LuaRef (GameObject::*)(luabridge::LuaRef))&GameObject::AddComponent)
            .endClass();
    luabridge::getGlobalNamespace(lua_state)
            .deriveClass<Animator,Component>("Animator")
            .addConstructor<void (*) ()>()
            .endClass();
    luabridge::getGlobalNamespace(lua_state)
            .deriveClass<Camera,Component>("Camera")
            .addConstructor<void (*) ()>()
            .endClass();

    luaL_dofile(lua_state, "../a.lua");

    GameObject::Foreach([](GameObject* game_object){
        auto camera=game_object->GetComponent<Camera>();
        if (!camera){
            return;
        }

        camera->Update();
    });

//
//    {
//        luabridge::LuaRef player_table=luabridge::getGlobal(lua_state,"GameObject");
//        std::cout<<typeid(player_table).name()<<std::endl;// N9luabridge6LuaRefE
//
//        if(player_table.isInstance<GameObject>()){
//            GameObject* lua_go=player_table.cast<GameObject*>();
//            std::cout<<typeid(lua_go).name()<<std::endl;// P10GameObject
//        }
//    }
//
    lua_close(lua_state);

    return 0;
}
