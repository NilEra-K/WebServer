/* 资源模块头文件 */
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

/**
 * 判断文件是否存在
 * searchResource()
 * Params:
 * path: 资源的路径
*/
int searchResource(const char* path);

/**
 * 识别资源的类型
 * identifyResourceType()
 * Params:
 * path: 资源的路径
*/
int identifyResourceType(const char* path, char* type);

#endif // _RESOURCE_H_


