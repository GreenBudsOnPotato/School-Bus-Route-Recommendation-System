# 西南大学校车路线推荐系统-School-Bus-Route-Recommendation-System
一种基于Dijkstra算法的校车乘坐推荐系统。支持途径点添加、别名查找、模糊搜索、换乘方案推介。基础数据为西南大学（Southwest University）实际的校车运营线网图。<br>
***~~项目本质是作者的C语言程序设计的课程设计~~***

## 主要功能
![选择菜单](Resource/UI.png "选择菜单")
- 展示校车路线表
- 展示所有站点的别名
- 校车乘坐推荐
    - 同线路上的起点、目的地路线搜索（遍历搜索）
    - 不同线路上的起点、目的地换乘方案（Dijkstra）（默认输出五条，可修改源码调整）
    - 添加途径点
    - 模糊搜索、关键词搜索
    - 将检索结果输出到日志文件中

    **\*注：因性能问题，当前版本暂不支持在有途径点的情况下检索换乘方案**
- 站点、别名、周边建筑及地标添加
    - 模糊搜索、关键词搜索
    - 站点存在判断逻辑、别名存在判断逻辑
- 站点、别名、周边建筑及地标添加
    - 模糊搜索、关键词搜索

更多功能等待后续开发~~~

## 下载（预编译发行版本）
通过本页面 [releases](https://github.com/GreenBudsOnPotato/School-Bus-Route-Recommendation-System/releases) 选项卡下载
## 鸣谢
- 课设归档：西南大学计算机与信息科学学院 软件学院（College of Computer and Information Science, Southwest University; School of Software, Southwest University.）
![院徽](Resource/大logo-蓝字.png "院徽")
- 西南大学计算机与信息科学学院 软件学院 讲师 [盛泳潘](https://cis.swu.edu.cn/info/1017/4288.htm)
    - [中文主页](http://shengyp.riknow.cn/)
    - [英文主页](https://shengyp.github.io/)
    - [Github](https://github.com/shengyp)
## 免责声明
本项目基于 `GNU 通用公共许可证第 3 版（GPL-v3.0）`授权。我们不对其适用性、可靠性或准确性作出任何明示或暗示的保证。在法律允许的最大范围内, 作者和贡献者不承担任何因使用本软件而产生的直接、间接、偶然、特殊或后果性的损害赔偿责任。<br>
使用本项目需遵守所在地法律法规, 不得进行任何侵犯第三方知识产权的行为。因使用本项目而产生的数据和缓存应在24小时内清除, 超出 24 小时的使用需获得相关权利人的授权。

## 版权声明
Copyright © 2025 GreenBudsOnPotato.<br>
Copyright © 2025 Ryudoya Network Studio, Liangjiang New District, Chongqing.<br>
![个人图标](Resource/个人图标.png "个人图标")

原作者保留对本软件著作权的保护、追诉等权利。使用或修改本软件时需严格遵守`GPL-v3.0`内规定的内容进行声明或开源。

## 隐私政策 (Privacy policy)
本软件不存在需要联网的板块。我们不收集任何用户数据, 不使用任何遥测组件。
