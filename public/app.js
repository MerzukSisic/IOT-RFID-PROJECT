// Firebase konfiguracija
// Firebase konfiguracija
const firebaseConfig = {
    apiKey: "AIzaSyAbNx5w8lXcgb9kIiq7NuhGjs7ANKWAT_8",
    authDomain: "iot-project-f5585.firebaseapp.com",
    databaseURL: "https://iot-project-f5585-default-rtdb.firebaseio.com",
    projectId: "iot-project-f5585",
    storageBucket: "iot-project-f5585.firebasestorage.app",
    messagingSenderId: "168988614609",
    appId: "1:168988614609:web:9e4b1dbc3cfa59cf2c8af0"
  };
  // Inicijalizacija Firebase-a
firebase.initializeApp(firebaseConfig);
const database = firebase.database();

// Funkcija za formatiranje datuma i vremena
function formatDateTime(dateString) {
  const date = new Date(dateString);
  const minutes = String(date.getMinutes()).padStart(2, '0');
  const hours = String(date.getHours()).padStart(2, '0');
  const day = String(date.getDate()).padStart(2, '0');
  const month = String(date.getMonth() + 1).padStart(2, '0'); // Meseci su 0-indeksirani
  const year = date.getFullYear();

  return `${hours}:${minutes}, ${day}.${month}.${year}`;
}

// Funkcija za izračunavanje ukupnog vremena na poslu
function calculateHours(log) {
  if (log.checkIn && log.checkOut) {
    const checkIn = new Date(log.checkIn);
    const checkOut = new Date(log.checkOut);
    return ((checkOut - checkIn) / (1000 * 60 * 60)).toFixed(2);
  }
  return null;
}

// Funkcija za prikaz ikonica i poruke validacije
function updateValidationMessage(isValid, message) {
  const validationIcon = document.getElementById('validationIcon');
  const validationMessage = document.getElementById('validationMessage');

  if (isValid) {
    validationIcon.textContent = '✔';
    validationIcon.style.color = 'green';
    validationMessage.textContent = message;
  } else {
    validationIcon.textContent = '✘';
    validationIcon.style.color = 'red';
    validationMessage.textContent = message;
  }
}

// Funkcija za popunjavanje tabele
function loadEmployees() {
  const tableBody = document.getElementById('employeeTable');

  database.ref('/employees').once('value').then(employeeSnapshot => {
    const employees = employeeSnapshot.val();

    database.ref('/logs').once('value').then(logSnapshot => {
      const logs = logSnapshot.val() || {};

      tableBody.innerHTML = ''; // Očisti prethodne redove
      Object.keys(employees).forEach(employeeId => {
        const employee = employees[employeeId];
        const employeeLogs = logs[employeeId] || [];
        const lastLog = employeeLogs[employeeLogs.length - 1] || {};

        const row = document.createElement('tr');
        row.innerHTML = `
          <td>${employee.firstName}</td>
          <td>${employee.lastName}</td>
          <td>${employee.position}</td>
          <td>${employee.department}</td>
          <td>${lastLog.checkIn ? formatDateTime(lastLog.checkIn) : 'N/A'}</td>
          <td>${lastLog.checkOut ? formatDateTime(lastLog.checkOut) : 'N/A'}</td>
          <td>${calculateHours(lastLog) || 'N/A'}</td>
        `;
        tableBody.appendChild(row);
      });
    });
  });
}



function resetValidationMessage() {
  const validationIcon = document.getElementById('validationIcon');
  const validationMessage = document.getElementById('validationMessage');

  validationIcon.textContent = ''; // Ukloni ikonu
  validationMessage.textContent = '---'; // Resetuj poruku na podrazumevanu vrednost
}


var card = document.getElementById('cardId');
database.ref('/currentCard').on('value', (snapshot) => {
  if (snapshot.exists()) {
    card.value = snapshot.val(); // Postavlja trenutnu vrednost u input polje
  } else {
    card.value = "N/A"; // Ako nema podataka, postavlja default vrednost
  }
});
loadEmployees();


function validateCard(cardId) {
  if (!cardId) {
      updateValidationMessage(false, 'Kartica nije očitana!');
      return;
  }

  database.ref(`/employees/${cardId}`).once('value').then(snapshot => {
      if (snapshot.exists()) {
          const employee = snapshot.val();

          database.ref(`/logs/${cardId}`).once('value').then(logSnapshot => {
              const logs = logSnapshot.val() || [];
              const currentTime = new Date().toISOString();
              const lastLog = logs[logs.length - 1] || {};

              if (lastLog && !lastLog.checkOut) {
                  // Ako postoji prijava bez odjave, beleži odjavu
                  lastLog.checkOut = currentTime;
                  logs[logs.length - 1] = lastLog;

                  database.ref(`/logs/${cardId}`).set(logs).then(() => {
                      updateValidationMessage(true, `Zaposleni ${employee.firstName} ${employee.lastName} je uspešno odjavljen!`);
                  });
              } else {
                  // Ako nema prijave, beleži prijavu
                  const newLog = { checkIn: currentTime };
                  logs.push(newLog);

                  database.ref(`/logs/${cardId}`).set(logs).then(() => {
                      updateValidationMessage(true, `Zaposleni ${employee.firstName} ${employee.lastName} je uspešno prijavljen!`);
                  });
              }
          });
      } else {
          updateValidationMessage(false, 'Kartica nije validna!');
      }
  }).catch(error => {
      updateValidationMessage(false, 'Greška pri validaciji kartice!');
      console.error(error);
  });
}

// Automatsko praćenje kartice u Firebase-u
database.ref('/currentCard').on('value', snapshot => {
  if (snapshot.exists()) {
      const cardId = snapshot.val();
      console.log(`Očitana kartica: ${cardId}`); // Za debagovanje
      validateCard(cardId); // Automatski validiraj karticu
  } else {
      updateValidationMessage(false, 'Nema očitane kartice!');
  }
});

database.ref('/logs').on('value', snapshot => {
  console.log('Promena u logovima detektovana, osvežavam tabelu...');
  loadEmployees(); // Osveži tabelu prilikom svake promene u logovima
});


// Automatsko učitavanje tabele zaposlenih
loadEmployees();