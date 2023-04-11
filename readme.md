### 模型输入输出

## 模型初始化参数

1. 配置文件相对路径

## 模型输入

1. 动作号及参数
2. 其他实体信息EntityInfo
3. 开火事件FireEvent

## 模型输出

1. 自动生成的基本信息(阵营/ID/...)
2. 实体信息EntityInfo
3. 毁伤等级DAMAGE_LEVEL
4. 开火事件FireEvent

### 组件约束

1. Protection组件必须与Block和Coordinate一同出现
2. Damage组件必须与Block和Coordinate一同出现