﻿---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by captain.
--- DateTime: 5/16/2022 10:55 PM
---

require("lua_extension")

Camera=class("Camera",Component)

function Camera:ctor()
    Camera.super.ctor(self)
    ---@type Cpp.Camera
    self.Camera_cpp_instance=Cpp.Camera() --创建对应的C++实例
end

function Camera:set_game_object(game_object)
    Camera.super:set_game_object(game_object)
    self.Camera_cpp_instance:set_game_object(game_object.game_object_cpp_instance)--设置C++实例的game_object
end

function Camera:Awake()
    self.Camera_cpp_instance:Awake()
end

function Camera:Update()
    self.Camera_cpp_instance:Update()
end