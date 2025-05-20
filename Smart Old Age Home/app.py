from flask import Flask, render_template, request, redirect, session, url_for, flash, jsonify
import mysql.connector
from datetime import datetime
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os

app = Flask(__name__)
app.secret_key = '00000'

db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="Motaung@16_Neo",
    database="smart_house"
)
cursor = db.cursor(dictionary=True)

# Fetch sensor data
def get_sensor_data(table):
    cursor.execute(f"SELECT * FROM {table} ORDER BY timestamp DESC LIMIT 100")
    return cursor.fetchall()

# Generate graph with labels, title, and legend
def generate_graph(data, value_field, image_name, label):
    if not data:
        print(f"No data available for {label}")
        return

    x = []
    for row in data:
        ts = row['timestamp']
        # If ts is already a datetime object, no need to convert it further.
        if isinstance(ts, datetime):
            x.append(ts)
        else:
            x.append(datetime.strptime(ts, '%Y-%m-%d %H:%M:%S'))
            
    y = [row[value_field] for row in data]

    plt.figure(figsize=(8, 4))
    plt.plot(x, y, marker='o', linestyle='-', label=label, color='blue', linewidth=2, markersize=5)

    plt.title(f"{label} Over Time")
    plt.xlabel("Timestamp")
    plt.ylabel(f"{label} Value")
    plt.legend(loc="upper right")
    plt.grid(True, linestyle='--', alpha=0.7)
    
    plt.gcf().autofmt_xdate()
    plt.tight_layout()
    path = os.path.join('static', image_name)
    plt.savefig(path, dpi=300)
    plt.close()


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        role = request.form['role']
        confirm_password = request.form['confirm_password']

        if password != confirm_password:
            flash("Passwords do not match. Please try again.", "error")
            return redirect('/signup')

        cursor.execute("SELECT * FROM users WHERE username = %s", (username,))
        existing_user = cursor.fetchone()
        if existing_user:
            flash("Username already exists. Choose a different one.", "error")
            return redirect('/signup')

        cursor.execute("INSERT INTO users (username, password, role) VALUES (%s, %s, %s)", 
                       (username, password, role))
        db.commit()
        flash("Signup successful! You can now log in.", "success")
        return redirect('/login')

    return render_template('signup.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']

        cursor.execute("SELECT * FROM users WHERE username=%s", (username,))
        user = cursor.fetchone()

        if user and user['password'] == password:  # ⚠ Replace with hashed passwords for security later!
            session['user'] = user['username']
            session['role'] = user['role']

            if user['role'] == 'admin':
                return redirect('/admin')
            elif user['role'] in ['ldr1', 'ldr2', 'ldr3', 'ir', 'temperature', 'humidity']:
                return redirect(f'/{user["role"]}')
            else:
                return redirect('/default-page')
        
        flash("Invalid username or password!", "error")
        return redirect('/login')

    return render_template('login.html')

@app.route('/logout')
def logout():
    session.clear()
    return redirect('/')
@app.route('/admin')
def admin_page():
    if 'user' not in session or session['role'] != 'admin':
        return redirect('/login')

    sensor_data = {
        "ldr1": get_sensor_data("ldr_data1"),
        "ldr2": get_sensor_data("ldr_data2"),
        "ldr3": get_sensor_data("ldr_data3"),
        "pir": get_sensor_data("pir_data"),
        "temperature": get_sensor_data("temperature_data"),
        "humidity": get_sensor_data("humidity_data")
    }


    value_field_mapping = {
        "ldr1": "ldr",
        "ldr2": "ldr",
        "ldr3": "ldr",
        "pir": "pir",
        "temperature": "temperature",
        "humidity": "humidity"
    }

    for sensor, data in sensor_data.items():
        value_field = value_field_mapping.get(sensor, sensor)
        generate_graph(data, value_field, f"{sensor}_graph.png", f"{sensor.capitalize()} Readings")

    return render_template('admin.html')

@app.route('/admin/graphs')
def admin_graphs():
    if 'user' not in session or session['role'] != 'admin':
        return redirect('/login')

    sensor_graphs = [
        "ldr1_graph.png",
        "ldr2_graph.png",
        "ldr3_graph.png",
        "pir_graph.png",
        "temperature_graph.png",
        "humidity_graph.png"
    ]

    return render_template('admin_graphs.html', graphs=sensor_graphs)


@app.route('/<sensor>')
def sensor_page(sensor):
    sensor_map = {
        "ldr1": "ldr_data1",
        "ldr2": "ldr_data2",
        "ldr3": "ldr_data3",
        "pir": "pir_data",
        "temperature": "temperature_data",
        "humidity": "humidity_data"
    }

    # Map sensor keys to the column names used in the tables
    column_mapping = {
        "ldr1": "ldr",
        "ldr2": "ldr",
        "ldr3": "ldr",
        "pir": "pir",  # For PIR data, the value is stored in the "pir" column
        "temperature": "temperature",
        "humidity": "humidity"
    }

    if sensor not in sensor_map or 'user' not in session or session['role'] not in ['admin', sensor]:
        return redirect('/login')

    data = get_sensor_data(sensor_map[sensor])
    generate_graph(data, sensor_map[sensor].split("_")[0], f"{sensor}_graph.png", f"{sensor.capitalize()} Readings")
    
    return render_template(
        'sensor_template.html', 
        title=sensor.capitalize(), 
        table=data, 
        graph_img=f"{sensor}_graph.png",
        sensor_col=column_mapping[sensor]  # pass the correct column name
    )


@app.route('/data', methods=['POST'])
def receive_data():
    if request.is_json:
        data = request.get_json()
        now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
 
        table_mapping = {
            "ldr1": ("ldr_data1", "ldr"),
            "ldr2": ("ldr_data2", "ldr"),
            "ldr3": ("ldr_data3", "ldr"),
            "ir": ("pir_data", "pir"),
            "temperature": ("temperature_data", "temperature"),
            "humidity": ("humidity_data", "humidity")
        }

        for key, (table, col) in table_mapping.items():
            if key in data:
                query = f"INSERT INTO {table} ({col}, timestamp) VALUES (%s, %s)"
                cursor.execute(query, (data[key], now))
        
        db.commit()
        return jsonify({'status': 'success'}), 200

    return jsonify({'status': 'error', 'message': 'Invalid JSON'}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
