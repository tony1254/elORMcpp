#ifndef ELOQUENTORM_H
#define ELOQUENTORM_H

#include "MySQLConexion.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

/**
 * @brief Clase que representa un modelo genérico al estilo Eloquent.
 *
 * Permite definir una tabla y sus columnas para realizar operaciones CRUD
 * (crear, leer, actualizar y eliminar) de forma sencilla.
 *
 * Si se establece el valor de un campo en "--NO_SAVE--" se ignorará ese campo en las operaciones de create() y update().
 */
class EloquentORM {
private:
    MySQLConexion &db;
    string table;
    vector<string> columns;              // Lista de columnas (orden definida)
    map<string, string> attributes;      // Atributos: par campo-valor
    MYSQL *conn;
    
    // Función auxiliar para ejecutar una consulta y obtener el resultado
    MYSQL_RES* execute(const string &query) {
        if(mysql_query(conn, query.c_str())){
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return nullptr;
        }
        return mysql_store_result(conn);
    }
public:
    // Valor marcador para indicar que un campo no se quiere guardar.
    static const string IGNORE_MARKER;
    
    /**
     * @brief Constructor.
     *
     * @param connection Referencia a la conexión MySQL.
     * @param tableName Nombre de la tabla.
     * @param cols Vector de nombres de columnas.
     */
    EloquentORM(MySQLConexion &connection, const string &tableName, const vector<string> &cols)
         : db(connection), table(tableName), columns(cols) {
         conn = db.getConnection();
         // Inicializar atributos con cadena vacía.
         for(auto &col: columns)
             attributes[col] = "";
    }
    
    /**
     * @brief Asigna un valor a un campo.
     */
    void set(const string &field, const string &value) {
         attributes[field] = value;
      //   if(find(columns.begin(), columns.end(), field) == columns.end()) {
          if(std::find(columns.begin(), columns.end(), field) == columns.end()) {
             columns.push_back(field);
         }
    }
    
    /**
     * @brief Obtiene el valor de un campo.
     */
    string get(const string &field) {
         return attributes[field];
    }
    
    /**
     * @brief Busca un registro por 'id' y carga sus atributos.
     *
     * @param id Valor del campo 'id'.
     * @return true si se encontró el registro; false en caso contrario.
     */
    bool find(int id) {
         string query = "SELECT * FROM " + table + " WHERE id = " + to_string(id) + " LIMIT 1";
         MYSQL_RES *res = execute(query);
         if(res) {
              MYSQL_ROW row = mysql_fetch_row(res);
              if(row) {
                   for(size_t i = 0; i < columns.size(); i++) {
                        attributes[columns[i]] = (row[i] ? row[i] : "");
                   }
                   mysql_free_result(res);
                   return true;
              }
              mysql_free_result(res);
         }
         return false;
    }
    
    /**
     * @brief Guarda el registro: crea uno nuevo si 'id' no está definido, o actualiza el existente.
     */
    bool save() {
         if(attributes.find("id") == attributes.end() || attributes["id"].empty())
              return create();
         else
              return update();
    }
    
    /**
     * @brief Inserta un nuevo registro en la tabla, ignorando los campos con valor IGNORE_MARKER.
     */
    bool create() {
         stringstream ss;
         ss << "INSERT INTO " << table << " (";
         // Solo se guardarán los campos cuyo valor no sea IGNORE_MARKER.
         vector<string> saveColumns;
         for(auto &col : columns){
             if(attributes[col] == IGNORE_MARKER)
                 continue;
             saveColumns.push_back(col);
         }
         for(size_t i = 0; i < saveColumns.size(); i++){
              ss << saveColumns[i];
              if(i < saveColumns.size()-1) ss << ", ";
         }
         ss << ") VALUES (";
         for(size_t i = 0; i < saveColumns.size(); i++){
              ss << "'" << attributes[saveColumns[i]] << "'";
              if(i < saveColumns.size()-1) ss << ", ";
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
     * @brief Actualiza el registro actual (requiere que 'id' esté definido), ignorando los campos con valor IGNORE_MARKER.
     */
    bool update() {
         if(attributes.find("id") == attributes.end() || attributes["id"].empty()){
              cerr << "Error al actualizar: 'id' no está definido." << endl;
              return false;
         }
         stringstream ss;
         ss << "UPDATE " << table << " SET ";
         bool first = true;
         for(auto &col : columns) {
              if(col == "id")
                  continue;
              if(attributes[col] == IGNORE_MARKER)
                  continue;
              if(!first)
                   ss << ", ";
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
     * @brief Obtiene todos los registros de la tabla.
     *
     * @return vector<map<string, string>> Vector de registros (cada registro es un mapa campo-valor).
     */
    vector< map<string, string> > getAll() {
         vector< map<string, string> > rows;
         string query = "SELECT * FROM " + table;
         MYSQL_RES *res = execute(query);
         if(res){
              MYSQL_ROW row;
              unsigned int num_fields = mysql_num_fields(res);
              while((row = mysql_fetch_row(res))){
                   map<string, string> record;
                   for(size_t i = 0; i < columns.size() && i < num_fields; i++){
                        record[columns[i]] = (row[i] ? row[i] : "");
                   }
                   rows.push_back(record);
              }
              mysql_free_result(res);
         }
         return rows;
    }
};

const string EloquentORM::IGNORE_MARKER = "--NO_SAVE--";

#endif // ELOQUENTORM_H
