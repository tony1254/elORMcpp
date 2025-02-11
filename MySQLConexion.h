#ifndef MYSQLCONEXION_H
#define MYSQLCONEXION_H

#include <mysql.h>
#include <string>
#include <iostream>

using namespace std;

/**
 * @brief Clase que gestiona la conexión a una base de datos MySQL.
 *
 * Proporciona métodos para abrir y cerrar la conexión, ejecutar consultas y obtener resultados.
 */
class MySQLConexion {
private:
    MYSQL *conn;
    string host;
    string user;
    string password;
    string database;
    unsigned int port;
public:
    /**
     * @brief Constructor.
     * 
     * @param user Nombre de usuario.
     * @param password Contraseña.
     * @param database Nombre de la base de datos.
     * @param host Host (por defecto: "localhost").
     * @param port Puerto (por defecto: 3306).
     */
    MySQLConexion(const string &user, const string &password, const string &database,
                  const string &host = "localhost", unsigned int port = 3306)
        : user(user), password(password), database(database), host(host), port(port) {
        conn = mysql_init(nullptr);
    }
    
    /**
     * @brief Abre la conexión a la base de datos.
     * 
     * @return true si la conexión es exitosa.
     * @return false en caso de error.
     */
    bool open() {
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), 
                                  database.c_str(), port, NULL, 0)) {
            cerr << "Error de conexión: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Cierra la conexión.
     */
    void close() {
        if(conn) {
            mysql_close(conn);
            conn = nullptr;
        }
    }
    
    /**
     * @brief Ejecuta una consulta (INSERT, UPDATE, DELETE).
     * 
     * @param query Consulta SQL a ejecutar.
     * @return true si la consulta se ejecutó correctamente.
     * @return false en caso de error.
     */
    bool executeQuery(const string &query) {
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Ejecuta una consulta SELECT y retorna el resultado.
     * 
     * @param query Consulta SQL SELECT.
     * @return MYSQL_RES* Puntero al resultado o nullptr si ocurre error.
     */
    MYSQL_RES* executeSelect(const string &query) {
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return nullptr;
        }
        return mysql_store_result(conn);
    }
    
    /**
     * @brief Retorna el puntero a la conexión MySQL.
     */
    MYSQL* getConnection() {
        return conn;
    }
    
    ~MySQLConexion() {
        close();
    }
};

#endif // MYSQLCONEXION_H
