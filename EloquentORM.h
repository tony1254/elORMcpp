#ifndef ELOQUENTORM_H
#define ELOQUENTORM_H

#include "MySQLConexion.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <mysql.h>

using namespace std;

/**
 * @brief Clase que representa un modelo genérico al estilo Eloquent para MySQL.
 *
 * Permite realizar operaciones CRUD y aplicar condiciones (WHERE) de forma sencilla, además de aceptar consultas personalizadas mediante raw().
 */
class EloquentORM {
private:
    MySQLConexion &db;
    string table;
    vector<string> columns;              // Lista de columnas (orden definida)
    map<string, string> attributes;      // Atributos: par campo-valor
    MYSQL *conn;
    string condition;                    // Condición WHERE construida con where()
    string rawQuery;                     // Consulta raw personalizada (si se establece)

    /**
     * @brief Función auxiliar para ejecutar una consulta SQL.
     *
     * @param query La consulta a ejecutar.
     * @return MYSQL_RES* Resultado de la consulta o nullptr en caso de error.
     */
    MYSQL_RES* execute(const string &query) {
        if(mysql_query(conn, query.c_str())){
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return nullptr;
        }
        return mysql_store_result(conn);
    }
public:
    /**
     * @brief Constructor.
     *
     * @param connection Referencia a la conexión MySQL.
     * @param tableName Nombre de la tabla.
     * @param cols Vector de nombres de columnas.
     */
    EloquentORM(MySQLConexion &connection, const string &tableName, const vector<string> &cols)
         : db(connection), table(tableName), columns(cols), condition(""), rawQuery("") {
         conn = db.getConnection();
         // Inicializar atributos con cadena vacía para cada columna.
         for(auto &col: columns)
             attributes[col] = "";
    }
    
    /**
     * @brief Asigna un valor a un campo.
     *
     * @param field Nombre del campo.
     * @param value Valor a asignar.
     */
    void set(const string &field, const string &value) {
         attributes[field] = value;
         // Si el campo no existe en el vector de columnas, se añade.
         if(std::find(columns.begin(), columns.end(), field) == columns.end()) {
             columns.push_back(field);
         }
    }
    
    /**
     * @brief Obtiene el valor de un campo.
     *
     * @param field Nombre del campo.
     * @return string Valor del campo.
     */
    string get(const string &field) {
         return attributes[field];
    }
    
    /**
     * @brief Busca un registro por el campo 'id' y carga sus atributos.
     *
     * @param id Valor del 'id' a buscar.
     * @return true Si se encontró el registro.
     * @return false Si no se encontró.
     */
    bool find(int id) {
         string query = "SELECT * FROM " + table + " WHERE id = " + to_string(id) + " LIMIT 1";
         MYSQL_RES *res = execute(query);
         if(res) {
              MYSQL_ROW row = mysql_fetch_row(res);
              if(row) {
                   unsigned int num_fields = mysql_num_fields(res);
                   MYSQL_FIELD *fields = mysql_fetch_fields(res);
                   for(unsigned int i = 0; i < num_fields; i++) {
                        attributes[string(fields[i].name)] = (row[i] ? row[i] : "");
                   }
                   mysql_free_result(res);
                   return true;
              }
              mysql_free_result(res);
         }
         return false;
    }
    
    /**
     * @brief Guarda el registro: crea uno nuevo si 'id' no está definido o actualiza el existente.
     *
     * @return true Si la operación fue exitosa.
     * @return false En caso de error.
     */
    bool save() {
         if(attributes.find("id") == attributes.end() || attributes["id"].empty())
              return create();
         else
              return update();
    }
    
    /**
     * @brief Inserta un nuevo registro en la tabla.
     *
     * @return true Si la inserción fue exitosa.
     * @return false En caso de error.
     */
    bool create() {
         stringstream ss;
         ss << "INSERT INTO " << table << " (";
         for(size_t i = 0; i < columns.size(); i++){
              ss << columns[i];
              if(i < columns.size()-1) ss << ", ";
         }
         ss << ") VALUES (";
         for(size_t i = 0; i < columns.size(); i++){
              ss << "'" << attributes[columns[i]] << "'";
              if(i < columns.size()-1) ss << ", ";
         }
         ss << ")";
         string query = ss.str();
         if(mysql_query(conn, query.c_str())){
              cerr << "Error creando registro: " << mysql_error(conn) << endl;
              return false;
         }
         return true;
    }
    
    /**
     * @brief Actualiza el registro actual (requiere que 'id' esté definido).
     *
     * @return true Si la actualización fue exitosa.
     * @return false En caso de error.
     */
    bool update() {
         if(attributes.find("id") == attributes.end() || attributes["id"].empty()){
              cerr << "Error al actualizar: 'id' no está definido." << endl;
              return false;
         }
         stringstream ss;
         ss << "UPDATE " << table << " SET ";
         bool first = true;
         for(auto &col: columns){
              if(col == "id") continue;
              if(!first) ss << ", ";
              ss << col << " = '" << attributes[col] << "'";
              first = false;
         }
         ss << " WHERE id = " << attributes["id"];
         string query = ss.str();
         if(mysql_query(conn, query.c_str())){
              cerr << "Error actualizando registro: " << mysql_error(conn) << endl;
              return false;
         }
         return true;
    }
    
    /**
     * @brief Elimina el registro actual (requiere que 'id' esté definido).
     *
     * @return true Si la eliminación fue exitosa.
     * @return false En caso de error.
     */
    bool remove() {
         if(attributes.find("id") == attributes.end() || attributes["id"].empty()){
              cerr << "Error al eliminar: 'id' no está definido." << endl;
              return false;
         }
         string query = "DELETE FROM " + table + " WHERE id = " + attributes["id"];
         if(mysql_query(conn, query.c_str())){
              cerr << "Error eliminando registro: " << mysql_error(conn) << endl;
              return false;
         }
         return true;
    }
    
    /**
     * @brief Aplica una condición WHERE para filtrar registros.
     *
     * Permite encadenar condiciones. Retorna una copia del objeto con la condición actualizada.
     *
     * @param field Nombre del campo.
     * @param value Valor a comparar.
     * @return EloquentORM Objeto con la condición aplicada.
     */
    EloquentORM where(const string &field, const string &value) {
         EloquentORM newORM = *this; // Copia del objeto actual
         string newCond = field + " LIKE '%" + value + "%'";
         if(!newORM.condition.empty()){
              newORM.condition += " AND " + newCond;
         } else {
              newORM.condition = newCond;
         }
         return newORM;
    }
    
    /**
     * @brief Permite asignar una consulta raw personalizada.
     *
     * Al usar este método, se ignoran las condiciones previamente establecidas.
     *
     * @param query Consulta SQL completa.
     * @return EloquentORM Objeto con la consulta raw asignada.
     */
    EloquentORM raw(const string &query) {
         EloquentORM newORM = *this;
         newORM.rawQuery = query;
         return newORM;
    }
    
    /**
     * @brief Obtiene todos los registros que cumplan la condición o, si se usó raw(), la consulta personalizada.
     *
     * @return vector< map<string, string> > Vector de registros (cada registro es un mapa campo-valor).
     */
    vector< map<string, string> > getAll() {
         vector< map<string, string> > rows;
         string query;
         if(!rawQuery.empty()){
              query = rawQuery;
         } else {
              query = "SELECT * FROM " + table;
              if(!condition.empty()){
                   query += " WHERE " + condition;
              }
         }
         MYSQL_RES *res = execute(query);
         if(res){
              MYSQL_ROW row;
              unsigned int num_fields = mysql_num_fields(res);
              MYSQL_FIELD *fields = mysql_fetch_fields(res);
              while((row = mysql_fetch_row(res))){
                   map<string, string> record;
                   for(unsigned int i = 0; i < num_fields; i++){
                        record[string(fields[i].name)] = (row[i] ? row[i] : "");
                   }
                   rows.push_back(record);
              }
              mysql_free_result(res);
         }
         return rows;
    }
    
    /**
     * @brief Obtiene el primer registro que cumpla la condición o de la consulta raw.
     *
     * @return map<string, string> Mapa con los campos y valores del primer registro encontrado.
     */
    map<string, string> first() {
         map<string, string> record;
         string query;
         if(!rawQuery.empty()){
              query = rawQuery;
         } else {
              query = "SELECT * FROM " + table;
              if(!condition.empty()){
                   query += " WHERE " + condition;
              }
         }
         query += " LIMIT 1";
         MYSQL_RES *res = execute(query);
         if(res) {
              MYSQL_ROW row = mysql_fetch_row(res);
              if(row){
                   unsigned int num_fields = mysql_num_fields(res);
                   MYSQL_FIELD *fields = mysql_fetch_fields(res);
                   for(unsigned int i = 0; i < num_fields; i++){
                        record[string(fields[i].name)] = (row[i] ? row[i] : "");
                   }
              }
              mysql_free_result(res);
         }
         return record;
    }
};

#endif // ELOQUENTORM_H
